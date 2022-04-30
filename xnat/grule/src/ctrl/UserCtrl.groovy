package ctrl

import cn.xnatural.app.ServerTpl
import cn.xnatural.http.ApiResp
import cn.xnatural.http.Ctrl
import cn.xnatural.http.HttpContext
import cn.xnatural.http.Path
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import cn.xnatural.remoter.Remoter
import entity.User
import entity.UserPrivilege
import service.rule.UserSrv

import javax.persistence.criteria.CriteriaBuilder
import javax.persistence.criteria.CriteriaQuery
import javax.persistence.criteria.Predicate
import javax.persistence.criteria.Root

import static entity.Privilege.*

/**
 * 用户, 权限相关接口
 */
@Ctrl(prefix = 'user')
class UserCtrl extends ServerTpl {
    @Lazy protected userSrv = bean(UserSrv)
    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')


    /**
     * 用户分页查询接口
     * @param hCtx
     * @param page >= 1
     * @param limit <= 50
     * @param id {@link User#id}
     * @param nameLike 搜索关键字
     */
    @Path(path = 'paging/{page}', method = 'get')
    ApiResp<Page<User>> paging(HttpContext hCtx, Integer page, Integer limit, String id, String nameLike) {
        if (limit && limit > 50) return ApiResp.fail("Param limit <=50")
        ApiResp.ok(
                repo.paging(User, page, limit?:8) {root, query, cb ->
                    query.orderBy(cb.asc(root.get('createTime')))
                    final ps = []

                    // 权限过虑: 只能查看自己能查看的用户
                    final subquery = query.subquery(UserPrivilege)
                    final root2 = subquery.from(UserPrivilege)
                    subquery.select(root2.get("resourceId"))
                    subquery.where(cb.and(cb.equal(root2.get("userId"), hCtx.getSessionAttr("uId")), cb.like(root2.get("privilegeId"), User_Read_.name() + (id?:'%'))))
                    ps << cb.in(root.get("id")).value(subquery)

                    if (nameLike) {
                        final arr = nameLike.split(" ").findAll {k -> k.trim()}.findAll {it}
                        if (arr) {
                            ps << cb.and(arr.findResults {k -> cb.like(root.get('name'), '%' + k + '%')}.toArray(new Predicate[arr.size()]))
                        }
                    }
                    ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
                }
        )
    }


    /**
     * 添加用户
     * @param name 用户名
     * @param password 密码md5
     */
    @Path(path = '/', method = 'put')
    ApiResp add(HttpContext hCtx, String name, String password) {
        hCtx.auth(User_Add.name())
        if (!name) return ApiResp.fail("Param name required")
        name = name.trim()
        if (name.length() < 2) return ApiResp.fail("Param name length >= 2")
        if (repo.exist(User, "name", name)) {
            return ApiResp.fail("Already exist name '${name}'")
        }
        if (!password) return ApiResp.fail("Param password required")
        password = password.trim()
        if (password.length() < 2) return ApiResp.fail("Param password length >= 2")

        final user = repo.saveOrUpdate(new User(name: name, password: password, creator: hCtx.getSessionAttr("uId", String)))
        ep.fire("user.created", user.id)

        // 为新用户添加默认权限
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Login.name(), privilegeName: Login.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Decision_Add.name(), privilegeName: Decision_Add.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Field_Read.name(), privilegeName: Field_Read.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Field_Add.name(), privilegeName: Field_Add.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Field_Update.name(), privilegeName: Field_Update.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Collector_Read.name(), privilegeName: Collector_Read.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Collector_Add.name(), privilegeName: Collector_Add.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: Collector_Update.name(), privilegeName: Collector_Update.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.Static, privilegeId: OpHistory_Read.name(), privilegeName: OpHistory_Read.cnName))
        repo.saveOrUpdate(new UserPrivilege(userId: user.id, type: Type.User, privilegeId: User_Read_.name() + user.id, privilegeName: User_Read_.cnName + user.name, resourceId: user.id))

        return ApiResp.ok().attr("id", user.id)
    }


    /**
     * 删除用户
     * @param uId 用户id
     */
    @Path(path = '{uId}', method = 'delete')
    ApiResp<Boolean> del(HttpContext hCtx, String uId) {
        hCtx.auth(User_Del_.name() + uId)
        if (!uId) return ApiResp.fail("Param uId required")
        final r = repo.delete(User, uId)
        ep.fire("user.delete", uId)
        return ApiResp.ok(r)
    }


    /**
     * 是否拥有某个权限
     * @param uId {@link entity.UserPrivilege#userId}
     * @param privilegeId {@link entity.UserPrivilege#privilegeId}
     */
    @Path(path = "{uId}/hasPrivilege/{privilegeId}", method = 'get')
    ApiResp<Boolean> hasPrivilege(HttpContext hCtx, String uId, String privilegeId) {
        hCtx.auth(Login.name())
        ApiResp.ok(userSrv.findPrivilege(uId, privilegeId) != null)
    }


    /**
     * 添加用户权限
     * @param uId 目标用户id
     * @param privilegeId 新增的权限标识集
     */
    @Path(path = "{uId}/addPrivilege/{privilegeId}", method = 'put')
    ApiResp addPrivilege(HttpContext hCtx, String uId, String privilegeId) {
        hCtx.auth(Grant.name(), User_Grant_.name() + uId)

        // 可以分配任意权限
        if (hCtx.hasAuth(Grant.name())) {
            if (!userSrv.attachUserPrivilege(uId, privilegeId)) {
                final up = repo.byAttr(UserPrivilege, "privilegeId", privilegeId)
                if (up == null) return ApiResp.fail("unrecognized privilege")
                repo.saveOrUpdate(new UserPrivilege(userId: uId, type: up.type, privilegeId: privilegeId, resourceId: up.resourceId, privilegeName: up.privilegeName))
                log.info("Add new privilege ${privilegeId}(${up.privilegeName}) for user $uId".toString())
            }
        }
        // 不是 Grant, 不能分配 Grant, User_Add, 和自己没有的权限
        else {
            if (Grant.name() == privilegeId) return ApiResp.fail("Not permission")

            // 不能分配 新的 User_Add 权限 给用户
            if (User_Add.name() == privilegeId && !userSrv.findPrivilege(uId, User_Add.name())) {
                return ApiResp.fail("Not permission")
            }
            // 验证是否都是可以分配的权限(只能分配自身拥有的权限)
            final up = userSrv.findPrivilege(hCtx.getSessionAttr("uId", String), privilegeId)
            if (!up) {
                return ApiResp.fail("Not allow grant privilege $privilegeId")
            }
            if (!userSrv.attachUserPrivilege(uId, privilegeId)) {
                // 新增
                final e = new UserPrivilege(userId: uId, type: up.type, privilegeId: privilegeId, privilegeName: up.privilegeName, resourceId: up.resourceId)
                repo.saveOrUpdate(e)
                log.info("Add new privilege ${e.privilegeId}(${e.privilegeName}) for user $uId".toString())
            }
        }

        userSrv.invalidUserPrivilegeCache(uId)
        bean(Remoter)?.dataVersion('user.privilege')?.update(uId, System.currentTimeMillis(), null)
        ApiResp.ok()
    }


    /**
     * 删除用户的权限
     * @param uId 用户id
     * @param privilegeId 要删除的权限标识
     */
    @Path(path = "{uId}/delPrivilege/{privilegeId}", method = 'delete')
    ApiResp delPrivilege(HttpContext hCtx, String uId, String privilegeId) {
        hCtx.auth(Grant.name(), User_Grant_.name() + uId)

        if (!hCtx.hasAuth(Grant.name())) {
            if (Grant.name() == privilegeId || User_Add.name() == privilegeId) {
                return ApiResp.fail("Not permission")
            }

            if (!userSrv.findPrivilege(hCtx.getSessionAttr("uId", String), privilegeId)) {
                return ApiResp.fail("Not permission")
            }
        }
        userSrv.detachUserPrivilege(uId, privilegeId)

        userSrv.invalidUserPrivilegeCache(uId)
        bean(Remoter)?.dataVersion('user.privilege')?.update(uId, System.currentTimeMillis(), null)
        ApiResp.ok()
    }


    /**
     * 密码修改
     * @param uId 用户id
     * @param newPassword 新密码 md5
     * @param oldPassword 旧密码
     */
    @Path(path = '{uId}/changePwd', method = 'post')
    ApiResp changePwd(HttpContext hCtx, String uId, String newPassword, String oldPassword) {
        if (!uId) return ApiResp.fail("Param uId required")
        if (hCtx.getSessionAttr("uId", String) != uId) {
            return ApiResp.of('403', "Not permission, only allow change self password")
        }
        if (!newPassword) return ApiResp.fail("Param newPassword required")
        if (newPassword.length() < 2) return ApiResp.fail("Param newPassword length > 1")

        final user = repo.byId(User, uId)
        if (!user) return ApiResp.fail("用户不存在")
        if (oldPassword != user.password) return ApiResp.fail('原密码错误')
        user.password = newPassword
        repo.saveOrUpdate(user)
        ep.fire("user.update", user.id)
        ApiResp.ok()
    }


    /**
     * 重置密码
     * @param newPassword md5后在密码
     */
    @Path(path = '{uId}/restPassword', method = 'post')
    ApiResp restPassword(HttpContext hCtx, String uId, String newPassword) {
        hCtx.auth(User_ResetPassword_.name() + uId)
        if (!uId) return ApiResp.fail("Param uId required")
        if (!newPassword) return ApiResp.fail("Param newPassword required")
        if (newPassword.length() < 2) return ApiResp.fail("Param newPassword length > 1")
        final user = repo.byId(User, uId)
        if (!user) return ApiResp.fail("用户不存在")

        user.password = newPassword
        repo.saveOrUpdate(user)

        ep.fire("user.update", user.id)
        ApiResp.ok()
    }


    /**
     * 分页查询用户权限
     * @param uId 用户id
     * @param page 第几页 >= 1
     * @param nameLike
     *      权限各关键词, 以空格分割多个关键词
     *      如果为空, 则匹配所有
     * @param limit 最多匹配个数
     * @param againstUid 用户id
     *      能为此用户还能分配的权限(结果不包含此用户的权限)
     *      有值: 搜索所有能分配的权限, 没值: 只搜自己有的权限
     */
    @Path(path = '{uId}/privilegePaging/{page}', method = 'get')
    ApiResp<Page<UserPrivilege>> privilegePaging(HttpContext hCtx, String uId, Integer page, Integer limit, String nameLike, String againstUid) {
        hCtx.auth(Login.name())
        final hasGrant = userSrv.findPrivilege(uId, Grant.name()) != null
        final kws = nameLike ? nameLike.split(" ").findAll {k -> k.trim()}.findAll {it} : null
        final conditionFn = {Root<UserPrivilege> root, CriteriaQuery<?> query, CriteriaBuilder cb ->
            final ps = []
            // Grant 为别的用户分配权限时 能查看所有权限
            if (!againstUid || !hasGrant) ps << cb.equal(root.get("userId"), uId)
            // 非 Grant 用户不能分配 User_Add
            if (againstUid && !hasGrant) ps << cb.notEqual(root.get("privilegeId"), User_Add.name())
            if (kws) {
                ps << cb.and(kws.findResults {k -> cb.like(root.get('privilegeName'), '%' + k + '%')}.toArray(new Predicate[kws.size()]))
            }
            if (againstUid) {
                final subquery = query.subquery(UserPrivilege)
                final root2 = subquery.from(UserPrivilege)
                subquery.select(root2.get("privilegeId"))
                subquery.where(cb.equal(root2.get("userId"), againstUid))
                ps << cb.in(root.get("privilegeId")).value(subquery).not()
            }
            ps
        }
        ApiResp.ok(
                repo.paging(UserPrivilege, page, limit?:10, {root, query, cb ->
                    query.groupBy(root.get("privilegeId")) // 过虑重复
                    query.orderBy(cb.asc(cb.selectCase()
                            .when(cb.equal(root.get("type"), Type.Static), 1)
                            .when(cb.equal(root.get("type"), Type.User), 2)
                            .when(cb.equal(root.get("type"), Type.Decision), 3)
                            .otherwise(4)), cb.asc(root.get("privilegeName")))
                    final ps = conditionFn(root, query, cb)
                    ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
                }, {root, query, cb ->
                    query.select(cb.countDistinct(root.get("privilegeId")))
                    final ps = conditionFn(root, query, cb)
                    ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
                })
        )
    }
}