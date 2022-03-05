from sqlalchemy import Column, String, Integer

from server.framework.core.models import CoreModel


class IdeRegistry(CoreModel):
    """
    IDE镜像源
    """
    __tablename__ = "ide_registry"

    name = Column(String(length=256), nullable=False, comment="镜像源名称", unique=True)
    registry = Column(String(length=256), nullable=False, comment="镜像源地址")
    enable = Column(Integer, nullable=False, default=1, comment="是否启用镜像源(0:禁用,1:启用,2:镜像源不可达)")
