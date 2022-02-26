from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

from server.framework.core.settings import settings

engine = create_engine(settings.db_url, echo=False,
                       pool_size=20, max_overflow=0,
                       pool_recycle=3600,
                       pool_pre_ping=True)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine, expire_on_commit=False)
Base = declarative_base()
