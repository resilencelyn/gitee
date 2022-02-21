from typing import Optional

from pydantic import BaseModel


class IdeBaseRequest(BaseModel):
    name: str
    create_user_id: str
    create_user_name: str
    ide_image_id: str
    # runtime: str
    # spec_node_name: str

    # volume_config: str
    # port_config: str

    ide_environment_id: str
    status: int = 0


class IdeCreateRequest(IdeBaseRequest):
    pass


class IdeUpdateRequest(IdeBaseRequest):
    id: str
