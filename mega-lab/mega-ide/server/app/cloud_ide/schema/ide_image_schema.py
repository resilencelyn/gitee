from typing import Optional

from pydantic import BaseModel


class IdeImageBaseRequest(BaseModel):
    name: str
    version: str
    ide_registry_id: str
    git_url: Optional[str]
    note: Optional[str]
    status: Optional[int] = 0
    ide_type: int


class IdeImageCreateRequest(IdeImageBaseRequest):
    pass


class IdeImageUpdateRequest(IdeImageBaseRequest):
    id: str
