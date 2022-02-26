import unittest

from server.framework.utils.kubernetes_manager import KubernetesManager


class KubernetesManagerTest(unittest.TestCase):

    def setUp(self) -> None:
        super().setUp()
        self.kubernetes_manager = KubernetesManager(kube_config_file='/Users/kira/.kube/config')

    def test_apply_argo_job(self):
        self.kubernetes_manager.run_build_image_workflow('./argo_simple_job.yml', 'sample', {})


if __name__ == '__main__':
    unittest.main()
