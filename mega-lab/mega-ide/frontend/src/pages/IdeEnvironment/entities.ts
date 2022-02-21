export interface IdeEnvironmentEntity extends BaseTableEntity {
  name: string;
  image_name: string;
  registry_id: string;
  registry_name: string;
  max_cpu: number;
  share_cpu: number;
  max_memory: number;
  share_memory: number;
  max_disk: number;
  assign_volume: number;
  ide_image_id: string;
  ide_image: string;
  status: number;
};

