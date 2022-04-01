package rpc

import (
	"context"
	"net/http"
	"vdns/lib/api/conv"
	"vdns/lib/api/errs"
	"vdns/lib/api/models"
)

func NewAliDNSRpc() VdnsRpc {
	return &AliDNSRpc{
		conv: &conv.AliDNSResponseConvert{},
	}
}

type AliDNSRpc struct {
	conv conv.VdnsResponseConverter
}

func (_this *AliDNSRpc) DoDescribeCtxRequest(_ context.Context, url string) (*models.DomainRecordsResponse, error) {
	resp, err := http.Get(url)
	if err != nil {
		return &models.DomainRecordsResponse{}, errs.NewApiErrorFromError(err)
	}
	return _this.conv.DescribeResponseConvert(resp)
}

func (_this *AliDNSRpc) DoCreateCtxRequest(_ context.Context, url string) (*models.DomainRecordStatusResponse, error) {
	resp, err := http.Get(url)
	if err != nil {
		return &models.DomainRecordStatusResponse{}, errs.NewApiErrorFromError(err)
	}
	return _this.conv.CreateResponseConvert(resp)
}

func (_this *AliDNSRpc) DoUpdateCtxRequest(_ context.Context, url string) (*models.DomainRecordStatusResponse, error) {
	resp, err := http.Get(url)
	if err != nil {
		return &models.DomainRecordStatusResponse{}, errs.NewApiErrorFromError(err)
	}
	return _this.conv.UpdateResponseConvert(resp)
}

func (_this *AliDNSRpc) DoDeleteCtxRequest(_ context.Context, url string) (*models.DomainRecordStatusResponse, error) {
	resp, err := http.Get(url)
	if err != nil {
		return &models.DomainRecordStatusResponse{}, errs.NewApiErrorFromError(err)
	}
	return _this.conv.DeleteResponseConvert(resp)
}

func (_this *AliDNSRpc) DoDescribeRequest(url string) (*models.DomainRecordsResponse, error) {
	return _this.DoDescribeCtxRequest(nil, url)
}

func (_this *AliDNSRpc) DoCreateRequest(url string) (*models.DomainRecordStatusResponse, error) {
	return _this.DoCreateCtxRequest(nil, url)
}

func (_this *AliDNSRpc) DoUpdateRequest(url string) (*models.DomainRecordStatusResponse, error) {
	return _this.DoUpdateCtxRequest(nil, url)
}

func (_this *AliDNSRpc) DoDeleteRequest(url string) (*models.DomainRecordStatusResponse, error) {
	return _this.DoDeleteCtxRequest(nil, url)
}
