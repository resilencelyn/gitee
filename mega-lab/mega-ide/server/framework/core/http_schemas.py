from typing import Optional, Dict, Any

from pydantic.dataclasses import dataclass


@dataclass
class CommonResponseSchema:
    message: Optional[str] = ""
    success: bool = True
    data: Optional[Any] = None


@dataclass
class OptionsResponseSchema:
    field: str
    value: str


@dataclass
class TableResponseSchema:
    total: int
    page: int
    success: bool = True
    data: Optional[Any] = None
