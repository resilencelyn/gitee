export interface NodeEntity extends BaseTableEntity {
  name: string;
  docker_version: string;
  gpu: number;
  ip: string;
  kernel_version: string;
  labels: { [key: string]: string };
  node_name: string;
  os_image: string;
  pods: any[];
};

