from pydantic import BaseModel


class OptionsSchema(BaseModel):
    value: str
    label: str
