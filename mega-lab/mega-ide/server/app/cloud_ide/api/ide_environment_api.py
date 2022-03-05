from typing import Optional, List

from fastapi import APIRouter, Depends, Body, Query
from sqlalchemy.orm import Session

from server.app.cloud_ide.model import IdeEnvironment, IdeImage, IdeRegistry
from server.app.cloud_ide.schema.ide_environment_schema import IdeEnvironmentCreateRequest, IdeEnvironmentUpdateRequest
from server.framework.core.http_schemas import CommonResponseSchema, TableResponseSchema
from server.framework.core.injects import get_db
from server.framework.core.schemas import OptionsSchema

ide_environment_api = APIRouter()


@ide_environment_api.get("/get_entity", name='获取环境配置实体')
async def get_entity(id: str = Query(None, description='环境配置id'),
                     db: Session = Depends(get_db)) -> IdeEnvironment:
    entity = db.query(IdeEnvironment).filter(id == IdeEnvironment.id).one()
    return entity


@ide_environment_api.get("/get_ide_environment_options", name='获取环境配置下拉选项')
async def get_ide_environment_options(db: Session = Depends(get_db)) -> List[OptionsSchema]:
    query = db.query(IdeEnvironment).all()

    option_results: List[OptionsSchema] = []
    for row in query:
        option_results.append(OptionsSchema(value=str(row.id), label=row.name))
    return option_results


@ide_environment_api.get("/list", response_model=TableResponseSchema, name='获取环境配置列表')
async def list(name: Optional[str] = Query(None, description='过滤条件:环境配置名称'),
               current: Optional[int] = Query(0, description='当前页数'),
               pageSize: Optional[int] = Query(10, description='每页记录的数量'),
               db: Session = Depends(get_db)) -> TableResponseSchema:
    """
    获取环境配置列表
    :return:
    """
    query = db.query(IdeEnvironment)
    if name:
        query = query.filter(IdeEnvironment.name.like("%" + name + "%"))
    total = query.count()
    items = query.limit(pageSize).offset((current - 1) * pageSize).all()
    return TableResponseSchema(data=items, total=total, page=pageSize, success=True)


@ide_environment_api.post("/create", response_model=CommonResponseSchema, name='创建环境配置')
async def create(data: IdeEnvironmentCreateRequest = Body(None, description='环境配置创建实体'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    创建开发环境配置
    :return:
    """
    entity = IdeEnvironment(**data.dict())
    db.add(entity)
    db.commit()
    return CommonResponseSchema(data={'id': entity.id}, message=f'创建成功')


@ide_environment_api.get("/delete", response_model=CommonResponseSchema, name='删除环境配置')
async def delete(ids: str = Query(None, description='环境配置id，多条记录以逗号分割'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    删除开发环境配置
    :return:
    """
    db.query(IdeEnvironment).filter(IdeEnvironment.id.in_(ids.split(","))).delete(synchronize_session=False)
    db.commit()
    return CommonResponseSchema(data={}, message=f'删除成功')


@ide_environment_api.post("/edit", response_model=CommonResponseSchema, name='编辑环境配置')
async def edit(data: IdeEnvironmentUpdateRequest = Body(None, description='环境配置编辑实体'),
               db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    编辑开发环境配置
    :return:
    """
    db.query(IdeEnvironment).filter(IdeEnvironment.id == data.id).update(data.dict())
    db.commit()
    return CommonResponseSchema(data={}, message=f'编辑成功')
