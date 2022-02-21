from typing import Optional, List

from fastapi import APIRouter, Depends, Query, Body
from sqlalchemy.orm import Session

from server.app.cloud_ide.model import IdeRegistry
from server.app.cloud_ide.schema.ide_registry_schema import IdeRegistryCreateRequest, IdeRegistryUpdateRequest
from server.framework.core.http_schemas import CommonResponseSchema, TableResponseSchema
from server.framework.core.injects import get_db
from server.framework.core.schemas import OptionsSchema

ide_registry_api = APIRouter()


@ide_registry_api.get("/get_registry_options", name='获取镜像源下拉选项', response_model=List[OptionsSchema])
async def get_registry_options(db: Session = Depends(get_db)) -> List[OptionsSchema]:
    query = db.query(IdeRegistry).all()

    option_results = []
    for row in query:
        option_results.append(OptionsSchema(value=str(row.id), label=row.name))
    return option_results


@ide_registry_api.get("/get_entity", name='获取单个镜像源')
async def get_entity(id: str = Query(None, description='镜像源id'),
                     db: Session = Depends(get_db)) -> IdeRegistry:
    entity = db.query(IdeRegistry).filter(id == IdeRegistry.id).one()
    return entity


@ide_registry_api.get("/list", response_model=TableResponseSchema, name='镜像源列表')
async def list(
        name: str = Query(None, description='过滤条件:镜像源名称'),
        enable: int = Query(None, description='过滤条件:镜像源是否启用(0:禁用,1:启用)'),
        current: Optional[int] = Query(0, description='当前页数'),
        pageSize: Optional[int] = Query(10, description='每页记录的数量'),
        db: Session = Depends(get_db)) -> TableResponseSchema:
    """
    获取镜像源列表
    :return:
    """
    query = db.query(IdeRegistry)
    if name is not None:
        query = query.filter(IdeRegistry.name.like("%" + name + "%"))
    if enable is not None:
        query = query.filter(IdeRegistry.enable == enable)
    total = query.count()
    items = query.limit(pageSize).offset((current - 1) * pageSize).all()
    return TableResponseSchema(data=items, total=total, page=pageSize, success=True)


@ide_registry_api.post("/create", response_model=CommonResponseSchema, name='创建镜像源')
async def create(data: IdeRegistryCreateRequest = Body(None, description='镜像源创建请求'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    创建镜像源
    :return:
    """
    entity = IdeRegistry(**data.dict())
    db.add(entity)
    db.commit()
    return CommonResponseSchema(data={'id': entity.id}, message=f'创建成功')


@ide_registry_api.get("/delete", response_model=CommonResponseSchema, name='删除镜像源')
async def delete(
        ids: str = Query(None, description='镜像源id，多条记录以逗号分割'),
        db: Session = Depends(get_db)) -> CommonResponseSchema:
    db.query(IdeRegistry).filter(IdeRegistry.id.in_(ids.split(","))).delete(synchronize_session=False)
    db.commit()
    return CommonResponseSchema(data={}, message=f'删除成功')


@ide_registry_api.post("/edit", response_model=CommonResponseSchema, name='编辑镜像源')
async def edit(data: IdeRegistryUpdateRequest = Body(None, description='镜像源编辑实体'),
               db: Session = Depends(get_db)) -> CommonResponseSchema:
    db.query(IdeRegistry).filter(IdeRegistry.id == data.id).update(data.dict())
    db.commit()
    return CommonResponseSchema(data={}, message=f'编辑成功')
