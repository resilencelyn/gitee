import uuid

from sqlalchemy import Column
from sqlalchemy_utils import Timestamp, UUIDType

from server.framework.core.database import Base


class CoreModel(Base, Timestamp):
    __abstract__ = True

    id = Column(UUIDType(binary=False), primary_key=True, index=True, default=uuid.uuid4)
