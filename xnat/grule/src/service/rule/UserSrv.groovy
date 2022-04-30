package service.rule

import cn.xnatural.app.CacheSrv
import cn.xnatural.app.ServerTpl
import cn.xnatural.enet.event.EL
import cn.xnatural.jpa.Repo
import cn.xnatural.remoter.Remoter
import entity.Decision
import entity.Privilege
import entity.User
import entity.UserPrivilege

import java.time.Duration

import static entity.Privilege.*

/**
 * 用户权限相关
 */
class UserSrv extends ServerTpl {

    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')
    @Lazy protected cache = bean(CacheSrv)


    /**
     * 初始化 权限数据
     */
    @EL(name = "jpa_rule.started", async = true)
    protected void initDefaultUser() {
        // 初始化默认用户
        if (!repo.exist(User)) {
            [
                new User(name: getStr('defaultUser.name', 'admin'), password: getStr('defaultUser.password', 'admin'))
            ].each {u ->
                final pass = u.password
                u.password = pass.md5()
                repo.saveOrUpdate(u)
                Privilege.values().each { p ->
                    if (p.type == Type.Static) {
                        repo.saveOrUpdate(new UserPrivilege(userId: u.id, type: p.type, privilegeId: p.name(), privilegeName: p.cnName))
                    }
                }
                userDynPrivileges(u.id, u.name).each {p ->
                    final up = new UserPrivilege(userId: u.id, privilegeId: p.v1, type: Type.User, privilegeName: p.v2, resourceId: p.v3)
                    // 默认不给第一个用户自己删除自己的权限
                    if (p.v1 == (User_Del_.name() + u.id)) up.userId = null
                    repo.saveOrUpdate(up)
                }
                log.info("添加默认用户. username: " + u.name + ", password: " + pass)
            }
        }
        // 所有权限入表
        Privilege.values().each { p ->
            if (p.type == Type.Static) {
                if (!repo.exist(UserPrivilege, "privilegeId", p.name())) {
                    repo.saveOrUpdate(new UserPrivilege(type: p.type, privilegeId: p.name(), privilegeName: p.cnName))
                }
            }
        }
    }


    // 远程节点过来事件, 通知用户数据变了
    @EL(name = ['user.dataVersion'], async = true)
    protected void userDataVersion(String id) { invalidUserCache(id) }

    // 远程节点过来事件, 通知用户权限数据变了
    @EL(name = 'user.privilege.dataVersion', async = true)
    protected void userPrivilegeDataVersion(String id) { invalidUserPrivilegeCache(id) }


    @EL(name = ["user.created", "user.update", "user.delete"])
    protected void userChangeEffect(String id) {
        if (!id) throw new IllegalArgumentException("");
        final user = repo.byId(User, id)

        if (user) {
            if (user.creator) { // 创建者拥有对他创建的用户所有动态权限
                userDynPrivileges(id, user.name).each {p ->
                    final up = repo.byAttr(UserPrivilege, "userId", user.creator, "privilegeId", p.v1)
                    if (up) { // 更新权限名
                        up.privilegeName = p.v2
                        repo.saveOrUpdate(up)
                    } else { // 新增
                        repo.saveOrUpdate(new UserPrivilege(userId: user.creator, type: Type.User, privilegeId: p.v1, privilegeName: p.v2, resourceId: p.v3))
                        log.info("Add privilege $p.v1($p.v2) for user: $user.creator".toString())
                    }
                }
            }
        } else {
            userDynPrivileges(id).each {p ->
                repo.trans {ses ->
                    ses.createQuery("delete from UserPrivilege where privilegeId=:pId").setParameter("pId", p.v1).executeUpdate()
                }
            }
        }
        invalidUserCache(id)
        async {
            // 通知集群中其它节点数据同步
            final remoter = bean(Remoter)
            remoter?.dataVersion('user')?.update(id, user ? user.updateTime.time : System.currentTimeMillis(), null)
            remoter?.dataVersion('user.privilege')?.update(id, System.currentTimeMillis(), null)
        }
    }


    // 决策创建完成 更新用户权限
    @EL(name = "decision.created")
    protected void decisionCreateAffect(String id) {
        final decision = repo.byId(Decision, id)
        if (!decision.creator) return
        final remoter = bean(Remoter)

        final dynPrivileges = decisionDynPrivileges(decision.id, decision.name)
        // 添加创建者的权限
        final user = repo.byId(User, decision.creator)
        dynPrivileges.each {p ->
            if (!repo.exist(UserPrivilege, "userId", user.id, "privilegeId", p.v1)) {
                repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Decision, privilegeId: p.v1, privilegeName: p.v2, resourceId: decision.id))
                log.info("Add privilege ${p.v1}(${p.v2}) for user ${user.name}".toString())
            }
        }
        invalidUserPrivilegeCache(user.id)
        async {
            remoter?.dataVersion('user.privilege')?.update(user.id, System.currentTimeMillis(), null)
        }

        async {
            // 继承创建者的的决策权限
            repo.rows(UserPrivilege) {root, query, cb ->
                cb.equal(root.get("privilegeId"), User_ExtendDecisionPrivilege_.name() + decision.creator)
            }.each {up ->
                dynPrivileges.each {p ->
                    if (!repo.exist(UserPrivilege, "userId", up.userId, "privilegeId", p.v1)) {
                        repo.saveOrUpdate(new UserPrivilege(userId: up.userId, type: Type.Decision, privilegeId: p.v1, privilegeName: p.v2, resourceId: p.v3))
                        log.info("Add privilege ${p.v1}(${p.v2}) for group user ${up.userId}".toString())
                    }
                }
                invalidUserPrivilegeCache(up.userId)
                // 通知集群中其它节点用户权限数据同步
                remoter?.dataVersion('user.privilege')?.update(up.userId, System.currentTimeMillis(), null)
            }
        }
    }


    @EL(name = "decision.delete", async = true)
    protected void decisionDeleteAffect(String id) {
        final Set<String> userIds = []
        // 删除用户关联决策的权限
        repo.rows(UserPrivilege) {root, query, cb ->
            root.get("privilegeId").in(decisionDynPrivileges(id).findResults {it.v1})
        }.each {up ->
            repo.delete(up)
            userIds << up.userId
        }
        userIds.each {uId ->
            invalidUserPrivilegeCache(uId)
            // 通知集群中其它节点用户权限数据同步
            bean(Remoter)?.dataVersion('user.privilege')?.update(uId, System.currentTimeMillis(), null)
        }
    }


    @EL(name = "decision.update", async = true)
    protected void decisionUpdateAffect(String id) {
        // 修改权限名
        queue('decision.update.privilegeName') {
            final decision = repo.byId(Decision, id)
            final dynPrivileges = decisionDynPrivileges(decision.id, decision.name)
            repo.rows(UserPrivilege) {root, query, cb ->
                root.get("privilegeId").in(dynPrivileges.findResults {it.v1})
            }.each {up ->
                final newName = dynPrivileges.find {it.v1 == up.privilegeId}.v2
                if (up.privilegeName != newName) {
                    up.privilegeName = newName
                    repo.saveOrUpdate(up)
                    // 通知集群中其它节点用户权限数据同步
                    bean(Remoter)?.dataVersion('user.privilege')?.update(up.userId, System.currentTimeMillis(), null)
                }
            }
        }
    }


    /**
     * 查找用户
     * @param id {@link User#id}
     * @return {@link User}
     */
    User getUser(String id) {
        if (!id) throw new IllegalArgumentException("Param id required")
        User u = cache?.getAndUpdate("user_" + id)
        if (u == null) {
            u = repo.byId(User, id)
            cache?.set("user_" + id, u, Duration.ofDays(3))
        }
        return u
    }


    void invalidUserCache(String uId) { cache?.remove("user_" + uId) }


    /**
     * 判断用户是否有某个权限
     * @param uId {@link User#id}
     * @param privilegeId {@link UserPrivilege#privilegeId}
     * @return {@link UserPrivilege}
     */
    UserPrivilege findPrivilege(String uId, String privilegeId) {
        repo.byAttr(UserPrivilege, "userId", uId, "privilegeId", privilegeId)
    }


    /**
     * 解除用户权限关联
     * @param uId 用户id
     * @param privilegeId 权限id
     */
    boolean detachUserPrivilege(String uId, String privilegeId) {
        final effect = repo.trans {ses ->
            ses.createQuery("update UserPrivilege set userId=null, updateTime=:time where userId=:uId and privilegeId=:pId")
                    .setParameter("uId", uId).setParameter("pId", privilegeId).setParameter("time", new Date()).executeUpdate()
        }
        if (effect) {
            log.info("Detached privilege ${privilegeId} from user $uId".toString())
            async{ cleanDetachedUserPrivilege() }
        }
        return effect
    }


    /**
     * 关联一个游离的权限给用户
     * @param uId 用户id
     * @param privilegeId 权限id
     */
    boolean attachUserPrivilege(String uId, String privilegeId) {
        final effect = repo.trans {ses ->
            final up = repo.byAttr(UserPrivilege, "userId", null, "privilegeId", privilegeId)
            if (up) {
                return ses.createQuery("update UserPrivilege set userId=:uId, updateTime=:time where id=:id and userId is null")
                        .setParameter("uId", uId).setParameter("id", up.id).setParameter("time", new Date())
                        .executeUpdate()
            }
            0
        }
        if (effect) {
            log.info("Attached privilege ${privilegeId} for user $uId".toString())
        }
        return effect
    }


    /**
     * 使用户权限缓存失效
     * @param uId 用户id
     */
    void invalidUserPrivilegeCache(String uId) { cache?.remove("privilege_" + uId) }


    /**
     * 清理多余的游离的权限记录
     * 注: 至少要保留一条(保证所有权限都至少有一条记录)
     */
    void cleanDetachedUserPrivilege() {
        repo.trans {ses ->
            ses.createQuery("""
                delete from UserPrivilege where id in (
                    select id from UserPrivilege where userId is null group by privilegeId having count(privilegeId) > :max
                )
            """.trim()).setParameter("max", getInteger("maxKeepDetachedUserPrivilege", 2)).executeUpdate()
        }
    }
}
