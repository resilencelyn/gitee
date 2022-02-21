from sqlalchemy import Integer, Column, String, ForeignKey, UniqueConstraint
from sqlalchemy_utils import UUIDType

from server.framework.core.models import CoreModel


class IdeEnvironment(CoreModel):
    """
    IDE环境
    """
    __tablename__ = "ide_environment"

    name = Column(String(length=256), unique=True, nullable=False, comment="环境名称")

    max_cpu = Column(Integer, nullable=False, default=1, comment="最大可用CPU数量")
    max_memory = Column(Integer, nullable=False, default=1, comment="最大可用内存(G)")
    max_disk = Column(Integer, nullable=False, default=10, comment="最大可用硬盘(G)")
    status = Column(Integer, default=1, nullable=False, comment="是否启用(0:否,1:是)")
