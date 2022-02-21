import { request } from 'umi';
import { IdeEntity } from '@/pages/Ide/entities';

export async function getIdeUrl(id: string | undefined) {
  return await request<IdeEntity>(`/api/cloud_ide/ide/get_ide_url?id=${id}`, {
    method: 'GET',
  });
}

export async function getIde(id: string | undefined) {
  return await request<IdeEntity>(`/api/cloud_ide/ide/get_entity?id=${id}`, {
    method: 'GET',
  });
}

export async function startIde(id: string | undefined) {
  return await request<IdeEntity>(`/api/cloud_ide/ide/start?id=${id}`, {
    method: 'GET',
  });
}

export async function stopIde(id: string | undefined) {
  return await request<IdeEntity>(`/api/cloud_ide/ide/stop?id=${id}`, {
    method: 'GET',
  });
}

export async function listIde(
  params: {
    current?: number;
    pageSize?: number;
  },
  options?: { [key: string]: any },
) {
  return request<{
    data: IdeEntity[];
    total?: number;
    success?: boolean;
  }>('/api/cloud_ide/ide/list', {
    method: 'GET',
    params: {
      ...params,
    },
    ...options,
  });
}

export async function addIde(
  data: { [key: string]: any },
  options?: { [key: string]: any },
) {
  return request<Record<string, any>>('/api/cloud_ide/ide/create', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function updateIde(
  data: { [key: string]: any },
  options?: { [key: string]: any },
) {
  return request<Record<string, any>>('/api/cloud_ide/ide/edit', {
    data,
    method: 'POST',
    ...(options || {}),
  });
}

export async function deleteIde(data: string, options?: { key: string[] }) {
  return request<Record<string, any>>(`/api/cloud_ide/ide/delete?ids=${data}`, {
    method: 'GET',
    ...(options || {}),
  });
}
