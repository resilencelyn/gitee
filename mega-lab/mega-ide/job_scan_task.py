from server.app.cloud_ide.model import IdeImage, Ide, IdeRegistry
from server.framework.core.database import SessionLocal
from server.framework.core.logger import logger
import requests

from server.framework.core.scheduler import scheduler
from server.framework.core.settings import settings


@scheduler.scheduled_job('interval', id='ide_status_scanner', seconds=5)
def ide_status_scanner():
    logger.info('扫描IDE状态....')
    db = SessionLocal()
    ides = db.query(Ide).all()
    for ide in ides:
        try:
            status = requests.get(f'http://{ide.id}.ide.cc', timeout=2000)
            if ide.status == 1 and status.status_code == 200:
                ide.status = 2
                db.commit()
            if ide.status == 3 and status.status_code != 200:
                ide.status = 0
                db.commit()
        except Exception:
            pass
    db.close()


@scheduler.scheduled_job('interval', id='build_image_scanner', seconds=5)
def build_image_scanner():
    logger.info('扫描镜像编译状态....')
    argo_job_status = requests.get(f'{settings.argo_url}/api/v1/workflows/{settings.kubernetes_namespace}').json()
    db = SessionLocal()
    images = db.query(IdeImage).filter(IdeImage.status == 1).all()

    for image in images:
        logger.info(f'检查镜像[{image.name}]')
        for job in argo_job_status['items']:
            if image.job_id == job['metadata']['name']:
                if job['status']['phase'] == 'Succeeded':
                    image.status = 3
                if job['status']['phase'] in ['Failed', 'Error']:
                    image.status = 2
                db.commit()
    db.close()


@scheduler.scheduled_job('interval', id='registry_scanner', seconds=5)
def registry_scanner():
    logger.info('扫描镜像源状态....')
    db = SessionLocal()
    registrys = db.query(IdeRegistry).filter(IdeRegistry.enable != 0).all()
    for registry in registrys:
        try:
            logger.info(f'检查镜像源[{registry.name}]')
            status = requests.get(f'http://{registry.registry}', timeout=2000)
            if registry.enable == 1 and status.status_code != 200:
                registry.enable = 2
                db.commit()
            if registry.enable == 2 and status.status_code == 200:
                registry.enable = 1
                db.commit()
        except Exception:
            pass
    db.close()


if __name__ == '__main__':
    scheduler.start()
