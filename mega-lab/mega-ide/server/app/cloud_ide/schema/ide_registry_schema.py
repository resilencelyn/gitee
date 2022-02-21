from typing import Optional

from pydantic import BaseModel


class IdeRegistryBaseRequest(BaseModel):
    name: str
    registry: str
    enable: Optional[int] = 1


class IdeRegistryCreateRequest(IdeRegistryBaseRequest):
    pass


class IdeRegistryUpdateRequest(IdeRegistryBaseRequest):
    id: str
