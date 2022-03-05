// Code generated by protoc-gen-go. DO NOT EDIT.
// source: douyu.proto

package douyu

import (
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

type ShowStatus int32

const (
	ShowStatus_Unknown  ShowStatus = 0
	ShowStatus_Living   ShowStatus = 1
	ShowStatus_NoLiving ShowStatus = 2
)

var ShowStatus_name = map[int32]string{
	0: "Unknown",
	1: "Living",
	2: "NoLiving",
}

var ShowStatus_value = map[string]int32{
	"Unknown":  0,
	"Living":   1,
	"NoLiving": 2,
}

func (x ShowStatus) String() string {
	return proto.EnumName(ShowStatus_name, int32(x))
}

func (ShowStatus) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_50f0928b53cdacf8, []int{0}
}

type VideoLoopStatus int32

const (
	VideoLoopStatus_Off VideoLoopStatus = 0
	VideoLoopStatus_On  VideoLoopStatus = 1
)

var VideoLoopStatus_name = map[int32]string{
	0: "Off",
	1: "On",
}

var VideoLoopStatus_value = map[string]int32{
	"Off": 0,
	"On":  1,
}

func (x VideoLoopStatus) String() string {
	return proto.EnumName(VideoLoopStatus_name, int32(x))
}

func (VideoLoopStatus) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_50f0928b53cdacf8, []int{1}
}

type Avatar struct {
	Big                  string   `protobuf:"bytes,1,opt,name=big,proto3" json:"big,omitempty"`
	Middle               string   `protobuf:"bytes,2,opt,name=middle,proto3" json:"middle,omitempty"`
	Small                string   `protobuf:"bytes,3,opt,name=small,proto3" json:"small,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *Avatar) Reset()         { *m = Avatar{} }
func (m *Avatar) String() string { return proto.CompactTextString(m) }
func (*Avatar) ProtoMessage()    {}
func (*Avatar) Descriptor() ([]byte, []int) {
	return fileDescriptor_50f0928b53cdacf8, []int{0}
}

func (m *Avatar) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_Avatar.Unmarshal(m, b)
}
func (m *Avatar) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_Avatar.Marshal(b, m, deterministic)
}
func (m *Avatar) XXX_Merge(src proto.Message) {
	xxx_messageInfo_Avatar.Merge(m, src)
}
func (m *Avatar) XXX_Size() int {
	return xxx_messageInfo_Avatar.Size(m)
}
func (m *Avatar) XXX_DiscardUnknown() {
	xxx_messageInfo_Avatar.DiscardUnknown(m)
}

var xxx_messageInfo_Avatar proto.InternalMessageInfo

func (m *Avatar) GetBig() string {
	if m != nil {
		return m.Big
	}
	return ""
}

func (m *Avatar) GetMiddle() string {
	if m != nil {
		return m.Middle
	}
	return ""
}

func (m *Avatar) GetSmall() string {
	if m != nil {
		return m.Small
	}
	return ""
}

type Room struct {
	Nickname             string          `protobuf:"bytes,1,opt,name=nickname,proto3" json:"nickname,omitempty"`
	RoomId               int64           `protobuf:"varint,2,opt,name=room_id,json=roomId,proto3" json:"room_id,omitempty"`
	RoomName             string          `protobuf:"bytes,3,opt,name=room_name,json=roomName,proto3" json:"room_name,omitempty"`
	RoomUrl              string          `protobuf:"bytes,4,opt,name=room_url,json=roomUrl,proto3" json:"room_url,omitempty"`
	ShowStatus           ShowStatus      `protobuf:"varint,5,opt,name=show_status,json=showStatus,proto3,enum=douyu.ShowStatus" json:"show_status,omitempty"`
	VideoLoop            VideoLoopStatus `protobuf:"varint,6,opt,name=videoLoop,proto3,enum=douyu.VideoLoopStatus" json:"videoLoop,omitempty"`
	Avatar               *Avatar         `protobuf:"bytes,7,opt,name=avatar,proto3" json:"avatar,omitempty"`
	XXX_NoUnkeyedLiteral struct{}        `json:"-"`
	XXX_unrecognized     []byte          `json:"-"`
	XXX_sizecache        int32           `json:"-"`
}

func (m *Room) Reset()         { *m = Room{} }
func (m *Room) String() string { return proto.CompactTextString(m) }
func (*Room) ProtoMessage()    {}
func (*Room) Descriptor() ([]byte, []int) {
	return fileDescriptor_50f0928b53cdacf8, []int{1}
}

func (m *Room) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_Room.Unmarshal(m, b)
}
func (m *Room) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_Room.Marshal(b, m, deterministic)
}
func (m *Room) XXX_Merge(src proto.Message) {
	xxx_messageInfo_Room.Merge(m, src)
}
func (m *Room) XXX_Size() int {
	return xxx_messageInfo_Room.Size(m)
}
func (m *Room) XXX_DiscardUnknown() {
	xxx_messageInfo_Room.DiscardUnknown(m)
}

var xxx_messageInfo_Room proto.InternalMessageInfo

func (m *Room) GetNickname() string {
	if m != nil {
		return m.Nickname
	}
	return ""
}

func (m *Room) GetRoomId() int64 {
	if m != nil {
		return m.RoomId
	}
	return 0
}

func (m *Room) GetRoomName() string {
	if m != nil {
		return m.RoomName
	}
	return ""
}

func (m *Room) GetRoomUrl() string {
	if m != nil {
		return m.RoomUrl
	}
	return ""
}

func (m *Room) GetShowStatus() ShowStatus {
	if m != nil {
		return m.ShowStatus
	}
	return ShowStatus_Unknown
}

func (m *Room) GetVideoLoop() VideoLoopStatus {
	if m != nil {
		return m.VideoLoop
	}
	return VideoLoopStatus_Off
}

func (m *Room) GetAvatar() *Avatar {
	if m != nil {
		return m.Avatar
	}
	return nil
}

type BetardResponse struct {
	Room                 *Room    `protobuf:"bytes,1,opt,name=room,proto3" json:"room,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *BetardResponse) Reset()         { *m = BetardResponse{} }
func (m *BetardResponse) String() string { return proto.CompactTextString(m) }
func (*BetardResponse) ProtoMessage()    {}
func (*BetardResponse) Descriptor() ([]byte, []int) {
	return fileDescriptor_50f0928b53cdacf8, []int{2}
}

func (m *BetardResponse) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_BetardResponse.Unmarshal(m, b)
}
func (m *BetardResponse) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_BetardResponse.Marshal(b, m, deterministic)
}
func (m *BetardResponse) XXX_Merge(src proto.Message) {
	xxx_messageInfo_BetardResponse.Merge(m, src)
}
func (m *BetardResponse) XXX_Size() int {
	return xxx_messageInfo_BetardResponse.Size(m)
}
func (m *BetardResponse) XXX_DiscardUnknown() {
	xxx_messageInfo_BetardResponse.DiscardUnknown(m)
}

var xxx_messageInfo_BetardResponse proto.InternalMessageInfo

func (m *BetardResponse) GetRoom() *Room {
	if m != nil {
		return m.Room
	}
	return nil
}

func init() {
	proto.RegisterEnum("douyu.ShowStatus", ShowStatus_name, ShowStatus_value)
	proto.RegisterEnum("douyu.VideoLoopStatus", VideoLoopStatus_name, VideoLoopStatus_value)
	proto.RegisterType((*Avatar)(nil), "douyu.Avatar")
	proto.RegisterType((*Room)(nil), "douyu.Room")
	proto.RegisterType((*BetardResponse)(nil), "douyu.BetardResponse")
}

func init() {
	proto.RegisterFile("douyu.proto", fileDescriptor_50f0928b53cdacf8)
}

var fileDescriptor_50f0928b53cdacf8 = []byte{
	// 344 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x5c, 0x51, 0x4d, 0x6b, 0xea, 0x50,
	0x10, 0x35, 0x89, 0xde, 0xe8, 0xe4, 0x3d, 0x5f, 0xde, 0x50, 0x6c, 0xda, 0x2e, 0x2a, 0x81, 0x82,
	0xb8, 0x10, 0x1a, 0xfb, 0x07, 0xea, 0xaa, 0x05, 0x51, 0x88, 0xd8, 0x45, 0x37, 0x12, 0x9b, 0xa8,
	0xc1, 0xe4, 0x8e, 0xe4, 0x43, 0xe9, 0xbe, 0x3f, 0xbc, 0x64, 0x72, 0xab, 0xd0, 0xdd, 0x9c, 0x73,
	0xee, 0x39, 0xcc, 0x3d, 0x03, 0x56, 0x48, 0xe5, 0x67, 0x39, 0x3a, 0x64, 0x54, 0x10, 0xb6, 0x18,
	0xb8, 0x2f, 0x20, 0x9e, 0x8f, 0x41, 0x11, 0x64, 0x68, 0x83, 0xb1, 0x8e, 0xb7, 0x8e, 0xd6, 0xd7,
	0x06, 0x1d, 0xbf, 0x1a, 0xb1, 0x07, 0x22, 0x8d, 0xc3, 0x30, 0x89, 0x1c, 0x9d, 0x49, 0x85, 0xf0,
	0x0a, 0x5a, 0x79, 0x1a, 0x24, 0x89, 0x63, 0x30, 0x5d, 0x03, 0xf7, 0x4b, 0x87, 0xa6, 0x4f, 0x94,
	0xe2, 0x2d, 0xb4, 0x65, 0xfc, 0xb1, 0x97, 0x41, 0x1a, 0xa9, 0xb4, 0x33, 0xc6, 0x6b, 0x30, 0x33,
	0xa2, 0x74, 0x15, 0x87, 0x9c, 0x69, 0xf8, 0xa2, 0x82, 0xaf, 0x21, 0xde, 0x41, 0x87, 0x05, 0x76,
	0xd5, 0xb9, 0xed, 0x8a, 0x98, 0x55, 0xae, 0x1b, 0xe0, 0x79, 0x55, 0x66, 0x89, 0xd3, 0x64, 0x8d,
	0x53, 0x96, 0x59, 0x82, 0x1e, 0x58, 0xf9, 0x8e, 0x4e, 0xab, 0xbc, 0x08, 0x8a, 0x32, 0x77, 0x5a,
	0x7d, 0x6d, 0xd0, 0xf5, 0xfe, 0x8f, 0xea, 0x9f, 0x2e, 0x76, 0x74, 0x5a, 0xb0, 0xe0, 0x43, 0x7e,
	0x9e, 0xf1, 0x09, 0x3a, 0xc7, 0x38, 0x8c, 0x68, 0x4a, 0x74, 0x70, 0x04, 0x3b, 0x7a, 0xca, 0xf1,
	0xf6, 0xc3, 0x2b, 0xdb, 0xe5, 0x21, 0x3e, 0x80, 0x08, 0xb8, 0x29, 0xc7, 0xec, 0x6b, 0x03, 0xcb,
	0xfb, 0xab, 0x2c, 0x75, 0x7d, 0xbe, 0x12, 0xdd, 0x47, 0xe8, 0x4e, 0xa2, 0x22, 0xc8, 0x42, 0x3f,
	0xca, 0x0f, 0x24, 0xf3, 0x08, 0xef, 0xa1, 0x59, 0x6d, 0xcb, 0x5d, 0x58, 0x9e, 0xa5, 0x6c, 0x55,
	0x55, 0x3e, 0x0b, 0xc3, 0x31, 0xc0, 0x65, 0x53, 0xb4, 0xc0, 0x5c, 0xca, 0xbd, 0xa4, 0x93, 0xb4,
	0x1b, 0x08, 0x20, 0xa6, 0xf1, 0x31, 0x96, 0x5b, 0x5b, 0xc3, 0x3f, 0xd0, 0x9e, 0x91, 0x42, 0xfa,
	0xd0, 0x85, 0x7f, 0xbf, 0x96, 0x45, 0x13, 0x8c, 0xf9, 0x66, 0x63, 0x37, 0x50, 0x80, 0x3e, 0x97,
	0xb6, 0x36, 0x31, 0xdf, 0xeb, 0x2b, 0xaf, 0x05, 0xdf, 0x7c, 0xfc, 0x1d, 0x00, 0x00, 0xff, 0xff,
	0xae, 0x9e, 0x9b, 0x5d, 0x02, 0x02, 0x00, 0x00,
}