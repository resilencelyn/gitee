import yaml
from kubernetes import config, client
from nanoid import generate

from server.app.cloud_ide.model import Ide, IdeEnvironment, IdeImage
from server.framework.core.logger import logger
from server.framework.core.settings import settings
from server.framework.core.template_loader import core_template


class KubernetesManager:
    def __init__(self, namespace: str = 'default'):
        """
        :param namespace: 操作的目标NameSpace
        :param kube_config_file: 目标KubeConfig，不填写则获取默认配置文件路径
        """
        self.namespace = namespace
        if settings.kube_config_file == '':
            config.load_kube_config()
        else:
            config.load_kube_config(config_file=settings.kube_config_file)

        self.core_api = client.CoreV1Api()
        self.app_api = client.AppsV1Api()
        self.custom_object_api = client.CustomObjectsApi()
        self.batch_api = client.BatchV1Api()
        self.traefik_resource_group = 'traefik.containo.us'

        self.argo_resource_group = 'argoproj.io'
        self.argo_resource_version = "v1alpha1"

    def delete_ide(self, uid: str, delete_pvc: bool):
        target_name = 'ide-' + uid
        try:
            logger.info(f'删除StatefulSet:[{target_name}]')
            self.app_api.delete_namespaced_stateful_set(namespace=self.namespace, name=target_name)
        except Exception:
            pass

        try:
            logger.info(f'删除Service:[{target_name}]')
            self.core_api.delete_namespaced_service(namespace=self.namespace, name=target_name)
        except Exception:
            pass

        try:
            logger.info(f'删除IngressRoute:[{target_name}]')
            self.custom_object_api.delete_namespaced_custom_object(group=self.traefik_resource_group,
                                                                   version="v1alpha1",
                                                                   plural="ingressroutes", namespace=self.namespace,
                                                                   name=target_name)
        except Exception:
            pass

        if delete_pvc:
            try:
                logger.info(f'删除卷:[{target_name}]')
                self.core_api.delete_namespaced_persistent_volume_claim(
                    namespace=self.namespace, name=target_name)
            except Exception:
                pass

    def start_ide(self, dynamic_dict: dict):
        dynamic_dict['namespace'] = self.namespace
        dynamic_dict['domain'] = settings.ide_domain

        try:
            pvc_template = core_template.get_template('ide/pvc.yml')
            pvc = pvc_template.render(dynamic_dict)
            self.core_api.create_namespaced_persistent_volume_claim(namespace=self.namespace,
                                                                    body=yaml.safe_load(pvc))
        except Exception:
            pass

        try:
            deployment_template = core_template.get_template('ide/deployment.yml')
            deployment = deployment_template.render(dynamic_dict)
            self.app_api.create_namespaced_stateful_set(
                namespace=self.namespace, body=yaml.safe_load(deployment))
        except Exception:
            pass

        try:
            svc_template = core_template.get_template('ide/svc.yml')
            svc = svc_template.render(dynamic_dict)
            self.core_api.create_namespaced_service(
                namespace=self.namespace, body=yaml.safe_load(svc))
        except Exception:
            pass

        try:
            ingress_template = core_template.get_template('ide/ingress.yml')
            ingress = ingress_template.render(dynamic_dict)
            self.custom_object_api.create_namespaced_custom_object(group=self.traefik_resource_group,
                                                                   version="v1alpha1",
                                                                   plural="ingressroutes",
                                                                   body=yaml.safe_load(ingress),
                                                                   namespace=self.namespace)
        except Exception:
            pass

    def run_build_image_workflow(self, dynamic_args: dict) -> str:
        """
        编译IDE镜像
        @param dynamic_args:
        @return:
        """
        workflow_id = generate('1234567890abcdef', 10)
        dynamic_args['uid'] = workflow_id
        dynamic_args['namespace'] = self.namespace
        template = core_template.get_template('argo/build-image.yml')
        result = template.render(dynamic_args)

        self.custom_object_api.create_namespaced_custom_object(group=self.argo_resource_group,
                                                               version=self.argo_resource_version,
                                                               plural="workflows",
                                                               body=yaml.safe_load(result),
                                                               namespace=self.namespace)
        return 'build-ide-' + workflow_id

    def list_node_info(self):
        """
        获取K8S中节点的信息
        """
        node_infos = []
        nodes = self.core_api.list_node()
        all_pods = self.core_api.list_pod_for_all_namespaces()
        for node in nodes.items:
            labels = node.metadata.labels
            pods = []
            for pod in all_pods.items:
                if pod.spec.node_name == labels['kubernetes.io/hostname'] and 'ide' in pod.spec.node_name:
                    pods.append({
                        'name': pod.metadata.name,
                        'namespace': pod.metadata.namespace,
                        'ip': pod.status.pod_ip,
                        'startup_time': str(pod.status.start_time)
                    })
            ip_address = ''
            for ip in node.status.addresses:
                if ip.type == 'InternalIP':
                    ip_address = ip.address

            gpu = 0
            if 'nvidia.com/gpu' in node.status.capacity:
                gpu = int(node.status.capacity['nvidia.com/gpu'])
            node_infos.append({
                'labels': labels,
                'node_name': labels['kubernetes.io/hostname'],
                'gpu': gpu,
                'docker_version': node.status.node_info.container_runtime_version,
                'kernel_version': node.status.node_info.kernel_version,
                'os_image': node.status.node_info.os_image,
                'ip': ip_address,
                'pods': pods
            })
        return node_infos
