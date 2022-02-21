from typing import Optional

from pydantic import BaseModel


class IdeEnvironmentBaseRequest(BaseModel):
    name: str
    max_cpu: int
    max_memory: int
    max_disk: int
    status: Optional[int] = 1


class IdeEnvironmentCreateRequest(IdeEnvironmentBaseRequest):
    pass


class IdeEnvironmentUpdateRequest(IdeEnvironmentBaseRequest):
    id: str
