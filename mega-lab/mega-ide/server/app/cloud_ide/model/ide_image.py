from sqlalchemy import Column, String, ForeignKey, UniqueConstraint, Integer
from sqlalchemy_utils import UUIDType

from server.framework.core.models import CoreModel


class IdeImage(CoreModel):
    """
    IDE镜像
    """
    __tablename__ = "ide_image"

    name = Column(String(length=256), nullable=False, comment="镜像名")
    version = Column(String(length=256), nullable=False, comment="镜像版本")
    git_url = Column(String(length=256), default='', comment="镜像的Git地址")
    note = Column(String(length=256), default='', comment="镜像简介")
    status = Column(Integer, default=0, nullable=False, comment="镜像状态(0:未构建,1:构建中,2:构建失败,3:构建成功)")

    job_id = Column(String(length=256), default='', comment="构建任务id")

    ide_registry_id = Column(UUIDType(binary=False), ForeignKey("ide_registry.id", ondelete='CASCADE'), comment="镜像源ID")
    ide_type = Column(Integer, comment="IDE镜像类型(0:jupyter,1:vscode)")
    __table_args__ = (UniqueConstraint('name', 'ide_registry_id', name='name_ide_registry_id'),)
