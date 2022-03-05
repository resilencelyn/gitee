from sqlalchemy import Integer, Column, Text, String, ForeignKey, UniqueConstraint
from sqlalchemy_utils import UUIDType

from server.framework.core.models import CoreModel


class Ide(CoreModel):
    """
    IDE
    """
    __tablename__ = "ide"

    name = Column(String(length=256), comment="IDE名称", nullable=False)
    create_user_id = Column(String(length=256), comment="创建用户ID")
    create_user_name = Column(String(length=256), comment="创建用户名称")
    # runtime = Column(Integer, default=1, comment="IDE运行的环境,(0:CPU,1:GPU)")
    # spec_node_name = Column(String(length=256), comment="指定运行的节点名称")
    # volume_config = Column(Text, comment="卷的挂载配置")
    # port_config = Column(Text, comment="端口的配置")
    ide_environment_id = Column(UUIDType(binary=False), ForeignKey("ide_environment.id"))
    ide_image_id = Column(UUIDType(binary=False), ForeignKey("ide_image.id"))
    status = Column(Integer, comment="当前状态(0:未运行,1:启动中,2:运行,3:暂停中)")

    __table_args__ = (UniqueConstraint('name', 'ide_environment_id', name='name'),)
