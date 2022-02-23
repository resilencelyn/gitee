package listx

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
)

/*
1. 双向链表
2. 支持头尾操作
3. 支持索引
4. 支持切片
*/

type ListxNode struct {
	next *ListxNode
	pre  *ListxNode
	val  *structure.Value
}

type Listx struct {
	head   *ListxNode
	tail   *ListxNode
	length int
}

func NewListXSingle() structure.ListXInterface {
	return &Listx{
		head:   nil,
		tail:   nil,
		length: 0,
	}
}

func (l *Listx) initByValue(val string) int {
	if l.head == nil && l.length == 0 {
		node := &ListxNode{
			val: structure.NewValue(val),
		}
		l.head = node
		l.tail = node
		l.length = 1

		return node.val.GetSize()
	}

	return 0
}

// 定位到 list 的元素， 支持反向索引
func (l *Listx) location(index int) (*ListxNode, error) {
	// 正定位
	if index >= 0 {
		node := l.head
		for ; index != 0 && node != nil; index -= 1 {
			node = node.next
		}

		if node == nil {
			return nil, errorx.New("index crosses the line")
		}
		return node, nil
	}

	node := l.tail
	for index = (-index) - 1; index != 0 && node != nil; {
		node = node.pre
		index -= 1
	}

	if node == nil {
		return nil, errorx.New("index crosses the line")
	}
	return node, nil

}

// 转换为左索引，负数索引
func (l *Listx) leftIndex(index int) (int, error) {
	if index < 0 && l.length+index > 0 {
		return index, nil
	}

	if index >= 0 && index < l.length {
		return index - l.length, nil
	}

	return 0, errorx.New("the index is not valid, index:%d", index)
}

// 转换为右索引，正数索引
func (l *Listx) rightIndex(index int) (int, error) {
	if index >= 0 && index < l.length {
		return index, nil
	}

	if index < 0 && l.length+index >= 0 {
		return l.length + index, nil
	}

	return 0, errorx.New("the index is not valid, index:%d", index)
}

func (l *Listx) remove(node *ListxNode) {

	if node == nil {
		return
	}

	l.length -= 1

	if node == l.head {
		l.head = node.next
		node.next.pre = nil
		return
	}

	if node == l.tail {
		l.tail = node.pre
		node.pre.next = nil
		return
	}

	node.pre.next = node.next
	node.next.pre = node.pre

}

func (l *Listx) SizeByte() int64 {
	bytes := 0
	r := l.head
	for r != nil {
		bytes += 16 + r.val.GetSize()
		r = r.next
	}
	return int64(bytes)
}

// RollBack TODO 事务相关, V2 实现
func (l *Listx) RollBack() error {
	panic("not implemented") // TODO: Implement
}

// Begin 事务相关, V2 实现
func (l *Listx) Begin() error {
	panic("not implemented") // TODO: Implement
}

// Comment 事务相关, V2 实现
func (l *Listx) Comment() error {
	panic("not implemented") // TODO: Implement
}

func (l *Listx) Encode() ([]byte, error) {
	panic("not implemented") // TODO: Implement
}

func (l *Listx) LPush(valueStr ...string) structure.UpdateLength {
	if len(valueStr) == 0 {
		return 0
	}

	// 使用第一个元素尝试初始化列表
	updateLength := l.initByValue(valueStr[0])
	for i := 1; i < len(valueStr); i++ {
		head := l.head
		val := structure.NewValue(valueStr[i])
		node := &ListxNode{val: val}
		node.next = head
		head.pre = node
		l.head = node
		l.length += 1
		updateLength += val.GetSize()
	}
	return structure.UpdateLength(updateLength)
}

func (l *Listx) RPush(valueStr ...string) structure.UpdateLength {
	if len(valueStr) == 0 {
		return 0
	}

	updateLength := l.initByValue(valueStr[0])
	for i := 1; i < len(valueStr); i++ {
		tail := l.tail
		val := structure.NewValue(valueStr[i])
		node := &ListxNode{val: val}
		tail.next = node
		node.pre = tail
		l.tail = node
		l.length += 1
		updateLength += val.GetSize()
	}

	return structure.UpdateLength(updateLength)
}

func (l *Listx) LPop(count int) ([]string, structure.UpdateLength) {
	values := make([]string, 0, count)

	head := l.head

	// 动态变化长度
	updateLength := 0

	for nodePoint := 0; head != nil && nodePoint < count; nodePoint++ {
		values = append(values, head.val.ToString())
		updateLength += head.val.GetSize()
		head = head.next
		l.length -= 1
	}

	if head != nil {
		head.pre = nil
	} else {
		l.tail = nil
	}

	l.head = head

	return values, structure.UpdateLength(updateLength)
}

func (l *Listx) RPop(count int) ([]string, structure.UpdateLength) {
	values := make([]string, 0, count)
	tail := l.tail

	updateLength := 0

	for nodePoint := 0; tail != nil && nodePoint < count; nodePoint++ {
		values = append(values, tail.val.ToString())
		updateLength += tail.val.GetSize()
		tail = tail.pre
		l.length -= 1
	}

	if tail != nil {
		tail.next = nil
	} else {
		l.head = nil
	}

	l.tail = tail

	return values, structure.UpdateLength(updateLength)
}

func (l *Listx) Index(index int) (string, error) {
	node, err := l.location(index)
	if err != nil {
		return "", nil
	}

	return node.val.ToString(), nil
}

// Insert, right 为 true 右添加，否则左添加
func (l *Listx) Insert(index int, right bool, valueStr ...string) (structure.UpdateLength, error) {
	targetNode, err := l.location(index)
	if err != nil {
		return 0, err
	}

	updateLength := 0
	for _, valStr := range valueStr {
		val := structure.NewValue(valStr)
		node := &ListxNode{val: val}

		// 右插
		if right {
			node.pre = targetNode
			node.next = targetNode.next

			targetNode.next = node
			targetNode.next.pre = node

			// 更新尾部
			if targetNode == l.tail {
				l.tail = node
			}

			targetNode = node
		} else {
			// 左插
			node.pre = targetNode.pre
			targetNode.pre.next = node

			node.next = targetNode
			targetNode.pre = node

			// 更新头部
			if targetNode == l.head {
				l.head = node
			}
			targetNode = node
		}

		updateLength += val.GetSize()
		l.length += 1
	}

	return structure.UpdateLength(updateLength), nil
}

func (l *Listx) Length() int {
	return l.length
}

// Slice 切片
func (l *Listx) Slice(start int, end int) (structure.UpdateLength, error) {

	startOffset, err := l.rightIndex(start)
	if err != nil {
		return 0, err
	}

	endRightOffset, err := l.rightIndex(end)
	if err != nil && end != l.length {
		return 0, errorx.New("index overstep the boundary, index: %d", end)
	}

	if startOffset >= endRightOffset && endRightOffset != 0 {
		return 0, errorx.New("the start index must be larger than the end index")
	}

	// 计算左偏移
	var endOffset int
	if end == l.length {
		endOffset = 0
	} else {
		endOffset, err = l.leftIndex(end)
		if err != nil {
			return 0, err
		}
	}

	updateLength := 0

	// 右切片
	head := l.head
	for nodePoint := 0; head != nil && nodePoint < startOffset; nodePoint++ {
		updateLength += head.val.GetSize()
		head = head.next
		l.length -= 1
	}
	l.head = head
	head.pre = nil

	tail := l.tail
	for nodePoint := 0; tail != nil && nodePoint < -endOffset; nodePoint++ {
		updateLength += tail.val.GetSize()
		tail = tail.pre
		l.length -= 1
	}

	l.tail = tail
	tail.next = nil

	return structure.UpdateLength(updateLength), nil
}

// Range 遍历
func (l *Listx) Range(start, end int) ([]string, error) {
	startOffset, err := l.rightIndex(start)
	if err != nil {
		return nil, err
	}

	endRightOffset, err := l.rightIndex(end)
	if err != nil && end != l.length {
		return nil, errorx.New("index overstep the boundary, index: %d", end)
	}

	if startOffset >= endRightOffset && endRightOffset != 0 {
		return nil, errorx.New("the start index must be larger than the end index")
	}

	head := l.head
	for nodePoint := 0; head != nil && nodePoint < startOffset; nodePoint++ {
		head = head.next
	}

	values := make([]string, 0)
	for i := 0; i < endRightOffset-startOffset && head != nil; i++ {
		values = append(values, head.val.ToString())
		head = head.next
	}
	return values, nil
}

func (l *Listx) Remove(value string, count int) (int, structure.UpdateLength) {

	if count == 0 {
		return 0, 0
	}

	updateLength := 0
	remCount := count

	// 头删除
	if count > 0 {
		node := l.head
		for node != nil && remCount > 0 {
			if node.val.ToString() == value {
				l.remove(node)
				remCount--
				updateLength += node.val.GetSize()
			}
			node = node.next
		}

		return count - remCount, structure.UpdateLength(updateLength)
	}

	// 尾删除
	node := l.tail
	for node != nil && remCount < 0 {
		if node.val.ToString() == value {
			l.remove(node)
			remCount++
			updateLength += node.val.GetSize()
		}
		node = node.pre
	}

	return remCount - count, structure.UpdateLength(updateLength)

}
