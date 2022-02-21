import { request } from 'umi';
import { IdeEnvironmentEntity } from '@/pages/IdeEnvironment/entities';

export async function getIdeEnvironment(id: string | undefined) {
  return await request<IdeEnvironmentEntity>(`/api/cloud_ide/ide_environment/get_entity?id=${id}`, {
    method: 'GET',
  });
}

export async function listIdeEnvironmentOptions() {
  return await request<[]>(`/api/cloud_ide/ide_environment/get_ide_environment_options`, {
    method: 'GET',
  });
}


export async function listIdeEnvironment(params: {
  current?: number;
  pageSize?: number;
}, options?: { [key: string]: any }) {
  return request<{
    data: IdeEnvironmentEntity[];
    total?: number;
    success?: boolean
  }>('/api/cloud_ide/ide_environment/list', {
    method: 'GET',
    params: {
      ...params,
    },
    ...options,
  });
}

export async function addIdeEnvironment(data: { [key: string]: any }, options?: { [key: string]: any }) {

  return request<Record<string, any>>('/api/cloud_ide/ide_environment/create', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function updateIdeEnvironment(data: { [key: string]: any }, options?: { [key: string]: any }) {
  return request<Record<string, any>>('/api/cloud_ide/ide_environment/edit', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function deleteIdeEnvironment(data: string, options?: { key: string[] }) {
  return request<Record<string, any>>(`/api/cloud_ide/ide_environment/delete?ids=${data}`, {
    method: 'GET',
    ...(options || {}),
  });
}
