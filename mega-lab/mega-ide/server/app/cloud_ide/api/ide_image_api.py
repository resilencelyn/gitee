from typing import Optional, List
import json
from fastapi import APIRouter, Depends, Query, Body
from nanoid import generate
from sqlalchemy.orm import Session
import requests
from server.app.cloud_ide.model import IdeImage, IdeRegistry
from server.app.cloud_ide.schema.ide_image_schema import IdeImageCreateRequest, IdeImageUpdateRequest
from server.framework.core.http_schemas import CommonResponseSchema, TableResponseSchema
from server.framework.core.injects import get_db
from server.framework.core.schemas import OptionsSchema
from server.framework.core.settings import settings
from server.framework.core.template_loader import core_template
from server.framework.utils.kubernetes_manager import KubernetesManager

ide_image_api = APIRouter()


@ide_image_api.get("/get_ide_image_options", name='获取镜像下拉选项列表', response_model=List[OptionsSchema])
async def get_ide_image_options(registry_id: str = Query(None, description='镜像源id'),
                                db: Session = Depends(get_db)) -> List[OptionsSchema]:
    query = db.query(IdeImage).filter(IdeImage.ide_registry_id == registry_id).all()

    option_results: List[OptionsSchema] = []
    for row in query:
        option_results.append(OptionsSchema(value=str(row.id), label=row.name))
    return option_results


@ide_image_api.get("/get_entity", name='获取镜像')
async def get_entity(id: str = Query(None, description='镜像id'), db: Session = Depends(get_db)) -> IdeImage:
    entity = db.query(IdeImage).filter(id == IdeImage.id).one()
    return entity


@ide_image_api.get("/list", response_model=TableResponseSchema, name='获取镜像列表')
async def list(name: Optional[str] = Query(None, description='过滤条件:镜像名称'),
               ide_registry_id: Optional[str] = Query(None, description='过滤条件:镜像源id'),
               current: Optional[int] = Query(0, description='当前页数'),
               pageSize: Optional[int] = Query(10, description='每页记录的数量'),
               db: Session = Depends(get_db)) -> TableResponseSchema:
    """
    获取IDE镜像列表
    :return:
    """
    query = db.query(IdeImage.id,
                     IdeImage.name,
                     IdeImage.note,
                     IdeImage.git_url,
                     IdeImage.status,
                     IdeImage.created,
                     IdeImage.version,
                     IdeRegistry.name.label('ide_registry'),
                     IdeRegistry.id.label('ide_registry_id'))
    query = query.join(IdeImage, IdeRegistry.id == IdeImage.ide_registry_id)
    if ide_registry_id is not None:
        query = query.filter(IdeRegistry.id == ide_registry_id)
    if name is not None:
        query = query.filter(IdeImage.name.like("%" + name + "%"))

    total = query.count()
    items = query.limit(pageSize).offset((current - 1) * pageSize).all()
    return TableResponseSchema(data=items, total=total, page=pageSize, success=True)


@ide_image_api.post("/create", response_model=CommonResponseSchema, name='创建镜像')
async def create(data: IdeImageCreateRequest = Body(None, description='镜像创建请求'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    创建IDE镜像配置
    :return:
    """
    entity = IdeImage(**data.dict())
    db.add(entity)
    db.commit()
    return CommonResponseSchema(data={'id': entity.id}, message=f'创建成功')


@ide_image_api.get("/delete", response_model=CommonResponseSchema, name='删除镜像')
async def delete(ids: str = Query(None, description='镜像id，多条记录以逗号分割'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    删除IDE镜像配置
    :return:
    """
    db.query(IdeImage).filter(IdeImage.id.in_(ids.split(","))).delete(synchronize_session=False)
    db.commit()
    return CommonResponseSchema(data={}, message=f'删除成功')


@ide_image_api.post("/edit", response_model=CommonResponseSchema, name='编辑镜像')
async def edit(data: IdeImageUpdateRequest = Body(None, description='镜像编辑实体'),
               db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    编辑IDE镜像配置
    :return:
    """
    db.query(IdeImage).filter(IdeImage.id == data.id).update(data.dict())
    db.commit()
    return CommonResponseSchema(data={}, message=f'编辑成功')


@ide_image_api.get("/show_logs", name='构建镜像', response_model=CommonResponseSchema)
async def show_logs(id: str = Query(None, description='镜像ID'),
                    db: Session = Depends(get_db)):
    entity: IdeImage = db.query(IdeImage).filter(IdeImage.id == id).one()

    argo_content = requests.get(
        f'{settings.argo_url}/api/v1/workflows/default/{entity.job_id}/log?logOptions.container=main&podName={entity.job_id}').text
    result = ''
    for i in argo_content.split('\n'):
        try:
            line_entity = json.loads(i)
            result += line_entity['result']['content'] + '\n'
        except:
            pass
    return CommonResponseSchema(
        data=result,
        message=''
    )


@ide_image_api.get("/build", name='构建镜像', response_model=CommonResponseSchema)
async def build(id: str = Query(None, description='镜像ID'),
                db: Session = Depends(get_db)):
    entity: IdeImage = db.query(IdeImage).filter(IdeImage.id == id).one()
    registry: IdeRegistry = db.query(IdeRegistry).filter(IdeRegistry.id == entity.ide_registry_id).one()

    dynamic_args = {
        'gitlab_url': entity.git_url,
        'image_name': f'{registry.registry}/{entity.name}:{entity.version}'
    }

    kubernetes_manager = KubernetesManager()
    workflow_id = kubernetes_manager.run_build_image_workflow(dynamic_args)

    entity.job_id = workflow_id
    entity.status = 1
    db.commit()
    return CommonResponseSchema(message='', data={})
