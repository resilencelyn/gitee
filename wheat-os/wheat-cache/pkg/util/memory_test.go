package util

import (
	"github.com/stretchr/testify/require"
	"testing"
)

func TestParseSizeToBit(t *testing.T) {
	size, err := ParseSizeToBit("18kb")
	require.NoError(t, err)
	require.Equal(t, size, int64(18*1024*8))

	size, err = ParseSizeToBit("18Mb")
	require.NoError(t, err)
	require.Equal(t, size, int64(18*1024*1024*8))

	size, err = ParseSizeToBit("18gB")
	require.NoError(t, err)
	require.Equal(t, size, int64(18*1024*1024*1024*8))

	size, err = ParseSizeToBit("18b")
	require.NoError(t, err)
	require.Equal(t, size, int64(18*8))

	size, err = ParseSizeToBit("18bit")
	require.NoError(t, err)
	require.Equal(t, size, int64(18*8))
}
