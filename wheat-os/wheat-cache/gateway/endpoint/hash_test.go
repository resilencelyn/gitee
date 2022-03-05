package endpoint

import (
	"testing"

	"github.com/stretchr/testify/require"
)

func TestHashTransport_GetTargetAddr(t *testing.T) {
	tran := NewHashEndpoint(3, nil, "127.0.0.1:5581", "127.0.0.1:5582", "127.0.0.1:5583")

	key := "test"

	target, err := tran.GetTargetAddr(key)
	require.NoError(t, err)
	require.Equal(t, target, "127.0.0.1:5582")
}
