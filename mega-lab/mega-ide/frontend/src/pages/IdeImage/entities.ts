export interface IdeImageEntity extends BaseTableEntity {
  name: string;
  version: string;
  ide_registry_id: string;
  ide_registry: string;
  git_url: string;
  note: string;
  status: number;
  ide_type: number;
};
