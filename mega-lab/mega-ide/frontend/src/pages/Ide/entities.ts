export interface IdeEntity extends BaseTableEntity {
  name: string;
  ide_environment_id: string;
  ide_environment: string;
  status: number;
  ide_registry: string;
  ide_registry_id: string;
  ide_image: string;
  ide_image_id: string;
};

