package skiplist

import (
	"crypto/rand"
	"fmt"
	"math"
	"math/big"
)

type skipListNode struct {
	score float64
	val   interface{}
	next  *skipListNode
	down  *skipListNode
	up    *skipListNode
	pre   *skipListNode
}

func newSkipListNode(score float64, val interface{}) *skipListNode {
	return &skipListNode{
		score: score,
		val:   val,
	}
}

type SkipList struct {
	head   *skipListNode
	tail   *skipListNode
	length int
	levels int // 单前层级

	maxLevels int //  最大层级
}

func NewSkipList(maxLevel int) *SkipList {
	skl := new(SkipList)
	skl.head = &skipListNode{score: math.MinInt64}
	skl.tail = &skipListNode{score: math.MaxInt64}

	skl.head.next = skl.tail
	skl.tail.pre = skl.head

	skl.length = 0
	skl.levels = 1
	skl.maxLevels = maxLevel

	return skl
}

func (s *SkipList) Length() int {
	return s.length
}

func (s *SkipList) Levels() int {
	return s.levels
}

func (s *SkipList) MaxLength() int {
	return s.maxLevels
}

// Insert 插入数据
func (s *SkipList) Insert(score float64, val interface{}) {

	node := newSkipListNode(score, val)
	f := s.searchNode(score)

	s.insertAfter(f, node)

	// 随机上升

	for newLevel := 1; ; newLevel++ {

		rander, _ := rand.Int(rand.Reader, big.NewInt(2))

		if rander.Int64() == 0 || newLevel >= s.maxLevels {
			break
		}

		// 上升层级
		if newLevel >= s.levels && s.levels < s.maxLevels {
			s.newLevels()
		}

		for f.up == nil {
			f = f.pre
		}

		f = f.up

		tmpNode := &skipListNode{score: score}
		node.up = tmpNode
		tmpNode.down = node
		s.insertAfter(f, tmpNode)

		node = tmpNode
	}

	s.length += 1
}

// Pop 弹出随机一个 score 值的 val
func (s *SkipList) Pop(score float64) interface{} {
	f := s.searchNode(score)
	if f.score == score {
		return nil
	}
	v := f.val

	s.delSkipListNode(f)
	return v
}

// Get 获取 随机一个 score 值的 val
func (s *SkipList) Get(score float64) interface{} {
	node := s.searchNode(score)
	if node != nil && node.score == score {
		return node.val
	}
	return nil
}

// GetAll 获取全部的 ALL
func (s *SkipList) GetAll(score float64) []interface{} {
	node := s.searchNode(score)
	values := make([]interface{}, 0)
	p := node

	// pre
	for p.score == score {
		values = append(values, p.val)
		p = p.pre
	}

	// next
	p = node.next
	for p.score == score {
		values = append(values, p.val)
		p = p.next
	}

	return values
}

// RemoveAll 删除全部
func (s *SkipList) RemoveAll(score float64) {
	node := s.searchNode(score)
	p := node
	delNode := make([]*skipListNode, 0)
	// pre
	for p.score == score {
		delNode = append(delNode, p)
		p = p.pre
	}

	// next
	p = node.next
	for p.score == score {
		delNode = append(delNode, p)
		p = p.next
	}
	s.delSkipListNode(delNode...)
}

func (s *SkipList) PopLeft(score float64) []interface{} {
	endPoint := s.tail

	head := s.head

	for endPoint.score > score {
		endPoint = endPoint.pre
	}

	var valuePoint *skipListNode
	for endPoint != nil {
		// 检查 end 后的节点情况
		for endPoint.next.score <= score {
			endPoint = endPoint.next
		}

		head.next = endPoint.next
		endPoint.next.pre = head
		valuePoint = endPoint

		endPoint = endPoint.down
		head = head.down
	}

	values := make([]interface{}, 0, 100)
	for valuePoint != nil && valuePoint.score > s.head.score {
		values = append(values, valuePoint.val)
		valuePoint = valuePoint.pre
	}

	return values

}

func (s *SkipList) PopRight(score float64) []interface{} {
	startPoint := s.head

	tail := s.tail

	for startPoint.score < score {
		startPoint = startPoint.next
	}

	var valuePoint *skipListNode
	for startPoint != nil {
		// 检查 end 后的节点情况
		for startPoint.pre.score >= score {
			startPoint = startPoint.pre
		}

		tail.pre = startPoint.pre
		startPoint.pre.next = tail
		valuePoint = startPoint

		startPoint = startPoint.down
		tail = tail.down
	}

	values := make([]interface{}, 0, 100)
	for valuePoint != nil && valuePoint.score < s.tail.score {
		values = append(values, valuePoint.val)
		valuePoint = valuePoint.next
	}

	return values

}

// PopRangeByScore 删除包括 start 到 end 的 score 的全部 value
func (s *SkipList) PopRangeByScore(startScore float64, endScore float64) []interface{} {
	if startScore >= endScore {
		return nil
	}

	startPoint := s.head
	endPoint := s.tail

	// 定位删除索引最高级
	for startPoint.score < startScore {
		startPoint = startPoint.next
	}
	for endPoint.score > endScore {
		endPoint = endPoint.pre
	}

	var valuePoint *skipListNode

	for startPoint != nil {
		// 检查 start 前的节点情况
		for startPoint.pre.score >= startScore {
			startPoint = startPoint.pre
		}

		// 检查 end 后的节点情况
		for endPoint.next.score <= endScore {
			endPoint = endPoint.next
		}

		// 删除节点
		valuePoint = startPoint
		startPoint.pre.next = endPoint.next
		endPoint.next.pre = startPoint.pre

		// 下一级处理
		startPoint = startPoint.down
		endPoint = endPoint.down
	}

	values := make([]interface{}, 0, 100)
	for valuePoint != nil && valuePoint.score <= endScore {
		values = append(values, valuePoint.val)
		valuePoint = valuePoint.next
	}

	return values
}

// 查找节点位置
func (s *SkipList) searchNode(score float64) *skipListNode {
	const (
		exitStatus = iota
		initStatus
		downStatus
		nextStatus
	)

	status := initStatus
	p := s.head

	for {
		switch status {
		case exitStatus:
			return p
		case initStatus:
			if p.score == score || p.next.score > score {
				status = downStatus
				continue
			}
			status = nextStatus

		case downStatus:
			if p.down == nil {
				status = exitStatus
				continue
			}
			p = p.down
			status = initStatus

		case nextStatus:
			// 不知名的 bug 保护一下
			if p.next.score != math.MaxInt64 {
				p = p.next
			}
			status = initStatus
		}
	}
}

// 在节点后插入新节点
func (s *SkipList) insertAfter(pNode *skipListNode, newNode *skipListNode) {
	if pNode == nil || newNode == nil {
		return
	}
	newNode.next = pNode.next
	newNode.pre = pNode
	pNode.next.pre = newNode
	pNode.next = newNode
}

// 添加 一个新的 levels 层
func (s *SkipList) newLevels() {
	newHead := &skipListNode{score: math.MinInt64}
	newTail := &skipListNode{score: math.MaxInt64}

	newHead.next = newTail
	newTail.pre = newHead

	s.head.up = newHead
	newHead.down = s.head
	s.tail.up = newTail
	newTail.down = s.tail

	s.head = newHead
	s.tail = newTail
	s.levels++
}

func (s *SkipList) debugPrint() {
	mapScore := make(map[float64]int)
	p := s.head
	for p.down != nil {
		p = p.down
	}
	index := 0
	for p != nil {
		mapScore[p.score] = index
		p = p.next
		index++
	}
	p = s.head
	for i := 0; i < s.levels; i++ {
		q := p
		preIndex := 0
		for q != nil {
			s := q.score
			if s == math.MinInt64 {
				fmt.Printf("%s", "BEGIN")
				q = q.next
				continue
			}
			index := mapScore[s]
			c := (index - preIndex - 1) * 6
			for m := 0; m < c; m++ {
				fmt.Print("-")
			}
			if s == math.MaxInt64 {
				fmt.Printf("-->%s\n", "END")
			} else {
				fmt.Printf("-->%3d", int(s))
				preIndex = index
			}
			q = q.next
		}
		p = p.down
	}
}

func (s *SkipList) delSkipListNode(sKm ...*skipListNode) {
	for _, f := range sKm {
		for f != nil {
			f.pre.next = f.next
			f.next.pre = f.pre
			f = f.up
		}
	}
}
