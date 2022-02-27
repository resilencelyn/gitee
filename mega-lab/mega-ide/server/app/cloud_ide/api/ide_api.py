from typing import Optional

from fastapi import APIRouter, Depends, Query, Body, Request
from keycloak import KeycloakOpenID
from logzero import logger
from sqlalchemy.orm import Session
from starlette.responses import JSONResponse

from server.app.cloud_ide.model import Ide, IdeEnvironment, IdeImage, IdeRegistry
from server.app.cloud_ide.schema.ide_schema import IdeCreateRequest, IdeUpdateRequest
from server.framework.core.http_schemas import CommonResponseSchema, TableResponseSchema
from server.framework.core.injects import get_db
from server.framework.core.settings import settings
from server.framework.core.template_loader import core_template
from server.framework.utils.kubernetes_manager import KubernetesManager

ide_api = APIRouter()


@ide_api.get("/ide_auth", name='校验当前用户是否能够访问ide')
async def ide_auth(request: Request,
                   db: Session = Depends(get_db)) -> Ide:
    try:
        keycloak_openid = KeycloakOpenID(server_url=settings.keycloak_url,
                                         client_id=settings.keycloak_client_id,
                                         realm_name=settings.keycloak_realm_name,
                                         client_secret_key="secret")
        token = request.cookies['token']
        user_info = keycloak_openid.userinfo(token)
        ide_id = request.url.hostname.split('.')[0]
        user_ide = db.query(Ide).filter(Ide.id == ide_id, Ide.create_user_id == user_info['sub']).one()
        if user_ide is not None:
            return JSONResponse({}, status_code=200)
        else:
            return JSONResponse({}, status_code=401)
    except:
        return JSONResponse({}, status_code=401)


@ide_api.get("/get_entity", name='获取ide实体')
async def get_entity(id: str = Query(None, description='开发环境id'),
                     db: Session = Depends(get_db)) -> Ide:
    entity = db.query(Ide.id, Ide.name, Ide.create_user_name,
                      Ide.ide_environment_id, Ide.status, Ide.created,
                      IdeEnvironment.name.label('ide_environment'),
                      IdeEnvironment.id.label('ide_environment_id'),
                      IdeImage.name.label('ide_image'),
                      IdeImage.id.label('ide_image_id'),
                      IdeRegistry.name.label('ide_registry'),
                      IdeRegistry.id.label('ide_registry_id')) \
        .join(IdeEnvironment, Ide.ide_environment_id == IdeEnvironment.id) \
        .join(IdeImage, Ide.ide_image_id == IdeImage.id) \
        .join(IdeRegistry, IdeImage.ide_registry_id == IdeRegistry.id) \
        .filter(id == Ide.id).one()
    return entity


@ide_api.get("/list", response_model=TableResponseSchema, name='获取IDE列表')
async def list(
        request: Request,
        name: Optional[str] = Query(None, description='过滤条件:ide名称'),
        current: Optional[int] = Query(0, description='当前页数'),
        pageSize: Optional[int] = Query(10, description='每页记录的数量'),
        db: Session = Depends(get_db)) -> TableResponseSchema:
    """
    获取IDE列表
    :return:
    """
    query = db.query(Ide.id, Ide.name, Ide.create_user_name,
                     Ide.ide_environment_id, Ide.status, Ide.created,
                     IdeEnvironment.name.label('ide_environment'),
                     IdeEnvironment.id.label('ide_environment_id'),
                     IdeImage.name.label('ide_image'),
                     IdeImage.id.label('ide_image_id'),
                     IdeRegistry.name.label('ide_registry'),
                     IdeRegistry.id.label('ide_registry_id')) \
        .join(IdeEnvironment, Ide.ide_environment_id == IdeEnvironment.id) \
        .join(IdeImage, Ide.ide_image_id == IdeImage.id) \
        .join(IdeRegistry, IdeImage.ide_registry_id == IdeRegistry.id)
    if name is not None:
        query = query.filter(Ide.name.like("%" + name + "%"))
    query = query.filter(Ide.create_user_id == request.state.user_id)
    total = query.count()
    items = query.limit(pageSize).offset((current - 1) * pageSize).all()
    return TableResponseSchema(data=items, total=total, page=pageSize, success=True)


@ide_api.post("/create", response_model=CommonResponseSchema, name='创建IDE')
async def create(request: Request,
                 data: IdeCreateRequest = Body(None, description='IDE创建实体'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    """
    创建IDE
    :return:
    """
    entity = Ide(**data.dict())
    entity.create_user_name = request.state.user_name
    entity.create_user_id = request.state.user_id
    db.add(entity)
    db.commit()
    return CommonResponseSchema(data={'id': entity.id}, message=f'创建成功')


@ide_api.get("/delete", response_model=CommonResponseSchema, name='删除IDE')
async def delete(ids: str = Query(None, description='镜像id，多条记录以逗号分割'),
                 db: Session = Depends(get_db)) -> CommonResponseSchema:
    kubernetes_manager = KubernetesManager()
    for id in ids.split(","):
        kubernetes_manager.delete_ide(id, True)
    db.query(Ide).filter(Ide.id.in_(ids.split(","))).delete(synchronize_session=False)
    db.commit()
    return CommonResponseSchema(data={}, message=f'删除成功')


@ide_api.post("/edit", response_model=CommonResponseSchema, name='编辑IDE')
async def edit(data: IdeUpdateRequest = Body(None, description='编辑IDE实体'),
               db: Session = Depends(get_db)) -> CommonResponseSchema:
    db.query(Ide).filter(Ide.id == data.id).update(data.dict())
    db.commit()
    return CommonResponseSchema(data={}, message=f'编辑成功')


@ide_api.get("/get_ide_url", name='获取IDE地址', response_model=CommonResponseSchema)
async def get_ide_url(id: str = Query(None, description='开发环境id'),
                      db: Session = Depends(get_db)):
    ide = db.query(Ide).filter(Ide.id == id).one()
    return CommonResponseSchema(data={'url': f'http://{ide.id}.{settings.ide_domain}'}, message=f'')


@ide_api.get("/start", name='启动IDE', response_model=CommonResponseSchema)
async def start(id: str = Query(None, description='开发环境id'),
                db: Session = Depends(get_db)):
    """
    启动IDE
    :return:
    """

    ide = db.query(Ide).filter(Ide.id == id).one()
    ide_environment: IdeEnvironment = db.query(IdeEnvironment).filter(IdeEnvironment.id == ide.ide_environment_id).one()
    ide_image: IdeImage = db.query(IdeImage).filter(IdeImage.id == ide.ide_image_id).one()
    ide_registry: IdeRegistry = db.query(IdeRegistry).filter(IdeRegistry.id == ide_image.ide_registry_id).one()

    ide_type: str = ''
    if ide_image.ide_type == 0:
        ide_type = 'jupyter'
    if ide_image.ide_type == 1:
        ide_type = 'vscode'
    dynamic_dict = {
        'id': ide.id,
        'storage_class': 'local-path',
        'storage_size': str(ide_environment.max_disk),
        'node_ports': '',
        'ide_type': ide_type,
        'volume_config': '',
        'node_name': '',
        'runtime': 'cpu',
        'extra_volume_config': '',
        'memory_limit': str(ide_environment.max_memory),
        'cpu_limit': str(ide_environment.max_cpu * 1000),
        'memory_request': '',
        'cpu_request': '',
        'image_name': ide_registry.registry + '/' + ide_image.name + ':' + ide_image.version
    }
    kubernetes_manager = KubernetesManager()
    kubernetes_manager.start_ide(dynamic_dict)
    ide.status = 1
    db.commit()
    return CommonResponseSchema(data={}, message=f'启动成功')


@ide_api.get("/stop", name='暂停IDE', response_model=CommonResponseSchema)
async def stop(id: str = Query(None, description='开发环境id'),
               db: Session = Depends(get_db)):
    """
    停止IDE
    :return:
    """
    kubernetes_manager = KubernetesManager()
    kubernetes_manager.delete_ide(id, False)

    ide = db.query(Ide).filter(Ide.id == id).one()
    ide.status = 3
    db.commit()
    return CommonResponseSchema(data={}, message=f'停止成功')
