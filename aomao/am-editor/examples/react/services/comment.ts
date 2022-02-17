import { Request } from '@aomao/engine';

const request = new Request();

export const list = () => {
	return request.ajax({
		url: `/api/comment/list`,
	});
};

export const remove = (payload: { render_id: string; id: number }) => {
	return request.ajax({
		url: `/api/comment/remove`,
		method: 'POST',
		data: payload,
	});
};

export const updateStatus = (payload: { ids: string; status: boolean }) => {
	return request.ajax({
		url: `/api/comment/updateStatus`,
		method: 'POST',
		data: payload,
	});
};

export const add = (payload: {
	title: string;
	render_id: string;
	content: string;
	username: string;
}) => {
	return request.ajax({
		url: `/api/comment/add`,
		method: 'POST',
		data: payload,
	});
};

export const update = (payload: {
	id: string;
	render_id: string;
	content: string;
}) => {
	return request.ajax({
		url: `/api/comment/update`,
		method: 'POST',
		data: payload,
	});
};
