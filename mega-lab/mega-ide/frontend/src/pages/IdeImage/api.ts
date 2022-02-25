import { request } from 'umi';
import { IdeImageEntity } from '@/pages/IdeImage/entities';

export async function getBuildInfo(id: string) {
  return await request(`/api/cloud_ide/ide_image/show_logs?id=${id}`, {
    method: 'GET',
  });
}

export async function buildIdeImage(id: string) {
  return await request<IdeImageEntity>(`/api/cloud_ide/ide_image/build?id=${id}`, {
    method: 'GET',
  });
}

export async function getIdeImage(id: string) {
  return await request<IdeImageEntity>(`/api/cloud_ide/ide_image/get_entity?id=${id}`, {
    method: 'GET',
  });
}

export async function listIdeImageOptions(registry_id: string) {
  return await request<[]>(`/api/cloud_ide/ide_image/get_ide_image_options?registry_id=${registry_id}`, {
    method: 'GET',
  });
}

export async function listIdeImage(params: {
  current?: number;
  pageSize?: number;
}, options?: { [key: string]: any }) {
  return request<{
    data: IdeImageEntity[];
    total?: number;
    success?: boolean
  }>('/api/cloud_ide/ide_image/list', {
    method: 'GET',
    params: {
      ...params,
    },
    ...options,
  });
}

export async function addIdeImage(data: { [key: string]: any }, options?: { [key: string]: any }) {
  return request<Record<string, any>>('/api/cloud_ide/ide_image/create', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function updateIdeImage(data: { [key: string]: any }, options?: { [key: string]: any }) {
  return request<Record<string, any>>('/api/cloud_ide/ide_image/edit', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function deleteIdeImage(data: string, options?: { key: string[] }) {
  return request<Record<string, any>>(`/api/cloud_ide/ide_image/delete?ids=${data}`, {
    method: 'GET',
    ...(options || {}),
  });
}
