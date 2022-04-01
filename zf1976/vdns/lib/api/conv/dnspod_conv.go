package conv

import (
	"context"
	"io/ioutil"
	"net/http"
	"vdns/lib/api/errs"
	"vdns/lib/api/models"
	"vdns/lib/api/models/dnspod_model"
	"vdns/lib/api/parameter"
	"vdns/lib/standard/record"
	"vdns/lib/util/convert"
	"vdns/lib/util/strs"
	"vdns/lib/util/vjson"
)

type DNSPodResponseConvert struct {
}

//goland:noinspection GoRedundantConversion
func (_this *DNSPodResponseConvert) DescribeResponseCtxConvert(ctx context.Context, resp *http.Response) (*models.DomainRecordsResponse, error) {
	if resp == nil {
		return nil, errs.NewVdnsError("*http.Response cannot been null.")
	}
	ctxDescribeRequest := new(models.DescribeDomainRecordsRequest)
	if ctx != nil {
		request := ctx.Value(parameter.DNSPOC_PARAMETER_CONTEXT_DESCRIBE_KEY)
		err := vjson.Convert(request, ctxDescribeRequest)
		if err != nil {
			return nil, err
		}
	}
	body := resp.Body
	defer body.Close()
	if resp.StatusCode == http.StatusOK {
		bytes, err := ioutil.ReadAll(body)
		if err != nil {
			return nil, errs.NewApiErrorFromError(err)
		}
		b := new(dnspod_model.DescribeRecordListResponse)
		err = vjson.ByteArrayConvert(bytes, b)
		if err != nil {
			return nil, errs.NewApiErrorFromError(err)
		}
		sourceResponse := b.Response
		if sourceResponse != nil {
			if sourceResponse.Error != nil {
				return nil, _this.errorBodyHandler(sourceResponse.Error, sourceResponse.RequestId)
			}
			dnspodRecords := sourceResponse.RecordList
			if dnspodRecords != nil || len(dnspodRecords) > 0 {
				records := make([]*models.Record, len(dnspodRecords))
				for i, dnspodRecord := range dnspodRecords {
					if dnspodRecord != nil {
						target := &models.Record{
							ID:         convert.AsString(dnspodRecord.RecordId),
							RecordType: record.Type(*dnspodRecord.Type),
							Domain:     ctxDescribeRequest.Domain,
							RR:         dnspodRecord.Name,
							Value:      dnspodRecord.Value,
							Status:     dnspodRecord.Status,
							TTL:        dnspodRecord.TTL,
						}
						records[i] = target
					}
				}
				var pageSize int64 = 100
				if ctxDescribeRequest.PageSize != nil {
					pageSize = *ctxDescribeRequest.PageSize
				}
				listCount := int64(len(records))
				response := &models.DomainRecordsResponse{
					TotalCount: sourceResponse.RecordCountInfo.TotalCount,
					PageSize:   &pageSize,
					PageNumber: ctxDescribeRequest.PageNumber,
					Records:    records,
					ListCount:  &listCount,
				}
				return response, nil
			}
		}
		return &models.DomainRecordsResponse{}, nil
	}
	return &models.DomainRecordsResponse{}, nil
}

func (_this *DNSPodResponseConvert) CreateResponseCtxConvert(_ context.Context, resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	if resp == nil {
		return nil, errs.NewVdnsError("*http.Response cannot been null.")
	}
	body := resp.Body
	defer body.Close()
	if resp.StatusCode == http.StatusOK {
		bytes, err := ioutil.ReadAll(body)
		if err != nil {
			return nil, errs.NewApiErrorFromError(err)
		}
		c := new(dnspod_model.CreateRecordResponse)
		err = vjson.ByteArrayConvert(bytes, c)
		if err != nil {
			return nil, err
		}
		sourceResponse := c.Response
		if sourceResponse.Error != nil {
			return nil, _this.errorBodyHandler(sourceResponse.Error, sourceResponse.RequestId)
		}
		response := &models.DomainRecordStatusResponse{
			RecordId:  convert.AsString(sourceResponse.RecordId),
			RequestId: sourceResponse.RequestId,
		}
		return response, nil
	} else {
		return nil, errs.NewVdnsError("dnspod bad response.")
	}
}

func (_this *DNSPodResponseConvert) UpdateResponseCtxConvert(_ context.Context, resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	if resp == nil {
		return nil, errs.NewVdnsError("*http.Response cannot been null.")
	}
	body := resp.Body
	defer body.Close()
	if resp.StatusCode == http.StatusOK {
		bytes, err := ioutil.ReadAll(body)
		if err != nil {
			return nil, errs.NewApiErrorFromError(err)
		}
		c := new(dnspod_model.ModifyRecordResponse)
		err = vjson.ByteArrayConvert(bytes, c)
		if err != nil {
			return nil, err
		}
		sourceResponse := c.Response
		if sourceResponse.Error != nil {
			return nil, _this.errorBodyHandler(sourceResponse.Error, sourceResponse.RequestId)
		}
		response := &models.DomainRecordStatusResponse{
			RecordId:  convert.AsString(sourceResponse.RecordId),
			RequestId: sourceResponse.RequestId,
		}
		return response, nil
	} else {
		return nil, errs.NewVdnsError("dnspod bad response.")
	}
}

func (_this *DNSPodResponseConvert) DeleteResponseCtxConvert(_ context.Context, resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	if resp == nil {
		return nil, errs.NewVdnsError("*http.Response cannot been null.")
	}
	body := resp.Body
	defer body.Close()
	if resp.StatusCode == http.StatusOK {
		bytes, err := ioutil.ReadAll(body)
		if err != nil {
			return nil, errs.NewApiErrorFromError(err)
		}
		c := new(dnspod_model.DeleteRecordResponse)
		err = vjson.ByteArrayConvert(bytes, c)
		if err != nil {
			return nil, err
		}
		sourceResponse := c.Response
		if sourceResponse.Error != nil {
			return nil, _this.errorBodyHandler(sourceResponse.Error, sourceResponse.RequestId)
		}
		response := &models.DomainRecordStatusResponse{
			RecordId:  convert.AsString("none"),
			RequestId: sourceResponse.RequestId,
		}
		return response, nil
	} else {
		return nil, errs.NewVdnsError("dnspod bad response.")
	}
}

func (_this *DNSPodResponseConvert) DescribeResponseConvert(resp *http.Response) (*models.DomainRecordsResponse, error) {
	return _this.DescribeResponseCtxConvert(nil, resp)
}

func (_this *DNSPodResponseConvert) CreateResponseConvert(resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	return _this.CreateResponseCtxConvert(nil, resp)
}

func (_this *DNSPodResponseConvert) UpdateResponseConvert(resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	return _this.UpdateResponseCtxConvert(nil, resp)
}

func (_this *DNSPodResponseConvert) DeleteResponseConvert(resp *http.Response) (*models.DomainRecordStatusResponse, error) {
	return _this.DeleteResponseCtxConvert(nil, resp)
}

func (_this *DNSPodResponseConvert) errorBodyHandler(e *dnspod_model.Error, requestId *string) error {
	return errs.NewApiErrorFromError(errs.NewTencentCloudSDKError(strs.StringValue(e.Code),
		strs.StringValue(e.Message), strs.StringValue(requestId)))
}
