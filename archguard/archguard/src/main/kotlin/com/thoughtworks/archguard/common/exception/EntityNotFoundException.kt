package com.thoughtworks.archguard.common.exception

class EntityNotFoundException : DomainException {
    constructor(message: String) : super(message)

    constructor(entityClass: Class<*>, id: Long?) : super("Cannot find the " + entityClass.simpleName + " with id " + id)
}
