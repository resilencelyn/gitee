from pydantic import BaseSettings


class Settings(BaseSettings):
    app_host: str = '0.0.0.0'
    app_port: int = 8000

    env: str = 'DEV'

    db_url: str = ''

    kubernetes_namespace: str = 'default'
    kube_config_file: str = ''
    argo_url: str = ''
    prometheus_address: str = 'http://prometheus.lab.cc'

    class Config:
        env_file = '.env'


settings = Settings()
