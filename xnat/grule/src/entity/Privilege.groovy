package entity

/**
 * 权限动作枚举
 */
enum Privilege {
    // 能修改所有用户的所有权限
    Grant(Type.Static, '权限管理'),
    Login(Type.Static, "登陆"),

    // 创建者默认拥有 User_Read_, User_Del_, User_ResetPassword_, User_Grant_, User_ExtendDecisionPrivilege_
    User_Add(Type.Static, "用户-创建"),
    User_Read_(Type.User, "用户-查看-"),
    User_Del_(Type.User, "用户-删除-"),
    User_ResetPassword_(Type.User, "用户-重置密码-"),
    // 能修改指定用户权限, 1. 修改用户有Grant, 则可修改所有权限; 2. 能为被修改用户添加修改用户的所有权限(除 User_Add); 3. 能删除被修改用户权限(修改用户所拥有的, 除 User_Add)
    User_Grant_(Type.User, "用户-修改权限-"),
    // 自动拥有别个用户创建的决策的动态权限
    User_ExtendDecisionPrivilege_(Type.User, "用户-继承决策权限-"),

    // 决策权限, 动态资源
    Decision_Add(Type.Static, "决策-创建"),
    Decision_Read_(Type.Decision, "决策-查看-"),
    Decision_Update_(Type.Decision, "决策-更新-"),
    Decision_Del_(Type.Decision, "决策-删除-"),

    Field_Add(Type.Static, "字段-添加"),
    Field_Read(Type.Static, "字段-查看"),
    Field_Update(Type.Static, "字段-更新"),
    Field_Del(Type.Static, "字段-删除"),

    Collector_Add(Type.Static, "收集器-添加"),
    Collector_Read(Type.Static, "收集器-查看"),
    Collector_Update(Type.Static, "收集器-更新"),
    Collector_Del(Type.Static, "收集器-删除"),

    OpHistory_Read(Type.Static, "操作历史查看")

    /**
     * 描述
     */
    String cnName
    /**
     * 权限类型
     */
    Type type

    Privilege(Type type, String cnName) {
        this.type = type
        this.cnName = cnName
    }


    /**
     * 用户动态权限生成
     * @param uId {@link User#id}
     * @return [privilegeId, privilegeName, resourceId]
     */
    static List<Tuple3<String, String, String>> userDynPrivileges(String uId, String uName = '') {
        if (!uId) throw new IllegalArgumentException("Param uId required")
        [
                [User_Read_.name() + uId, User_Read_.cnName + (uName?:uId), uId] as Tuple3,
                [User_Del_.name() + uId, User_Del_.cnName + (uName?:uId), uId] as Tuple3,
                [User_ResetPassword_.name() + uId, User_ResetPassword_.cnName + (uName?:uId), uId] as Tuple3,
                [User_Grant_.name() + uId, User_Grant_.cnName + (uName?:uId), uId] as Tuple3,
                [User_ExtendDecisionPrivilege_.name() + uId, User_ExtendDecisionPrivilege_.cnName + (uName?:uId), uId] as Tuple3,
        ]
    }


    /**
     * 决策动态权限生成
     * @param dId {@link Decision#id}
     * @return [privilegeId, privilegeName, resourceId]
     */
    static List<Tuple3<String, String, String>> decisionDynPrivileges(String dId, String decisionName = '') {
        if (!dId) throw new IllegalArgumentException("Param dId required")
        [
                [Decision_Del_.name() + dId, Decision_Del_.cnName + (decisionName?:dId), dId] as Tuple3,
                [Decision_Update_.name() + dId, Decision_Update_.cnName + (decisionName?:dId), dId] as Tuple3,
                [Decision_Read_.name() + dId, Decision_Read_.cnName + (decisionName?:dId), dId] as Tuple3,
        ]
    }

    enum Type {
        Decision, User, Static
    }
}