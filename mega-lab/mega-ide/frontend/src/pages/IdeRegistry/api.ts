import { request } from 'umi';
import { IdeRegistryEntity } from '@/pages/IdeRegistry/entities';

export async function listIdeRegistryOptions() {
  return await request<[]>('/api/cloud_ide/ide_registry/get_registry_options', {
    method: 'GET',
  });
}

export async function getIdeRegistry(id: string | undefined) {
  return await request<IdeRegistryEntity>(`/api/cloud_ide/ide_registry/get_entity?id=${id}`, {
    method: 'GET',
  });
}

export async function listIdeRegistry(params: {
  current?: number;
  pageSize?: number;
}, options?: { [key: string]: any }) {
  return request<{
    data: IdeRegistryEntity[];
    total?: number;
    success?: boolean
  }>('/api/cloud_ide/ide_registry/list', {
    method: 'GET',
    params: {
      ...params,
    },
    ...options,
  });
}

export async function addIdeRegistry(data: { [key: string]: any }, options?: { [key: string]: any }) {
  return request<Record<string, any>>('/api/cloud_ide/ide_registry/create', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function updateIdeRegistry(data: { [key: string]: any }, options?: { [key: string]: any }) {
  return request<Record<string, any>>('/api/cloud_ide/ide_registry/edit', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function deleteIdeRegistry(data: string, options?: { key: string[] }) {
  return request<Record<string, any>>(`/api/cloud_ide/ide_registry/delete?ids=${data}`, {
    method: 'GET',
    ...(options || {}),
  });
}
