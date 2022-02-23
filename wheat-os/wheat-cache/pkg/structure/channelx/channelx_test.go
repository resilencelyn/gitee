package channelx

import (
	"testing"

	"github.com/stretchr/testify/require"
)

func TestChannelX_Push(t *testing.T) {
	c := MakeChannelX(10)
	require.Equal(t, c.Length(), 0)

	up := c.Push("111")
	require.Equal(t, 24, int(up))

	res, up := c.Pop()
	require.Equal(t, -24, int(up))
	require.Equal(t, res, "111")

	up = c.Push("111")
	c.Clean()

	require.Equal(t, c.Length(), 0)
}
