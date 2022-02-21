from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

from apscheduler.schedulers.asyncio import AsyncIOScheduler
from apscheduler.jobstores.sqlalchemy import SQLAlchemyJobStore
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.schedulers.blocking import BlockingScheduler


def init_scheduler():
    job_defaults = {
        'coalesce': False,
        'max_instances': 1
    }
    return BlockingScheduler(job_defaults=job_defaults,
                             timezone='Asia/Shanghai')


scheduler: BlockingScheduler = init_scheduler()
