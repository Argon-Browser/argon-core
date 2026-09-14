// Test snap with echo invoke and snap.request() round-trip.
module.exports.onRpcRequest = async (request) => {
  const { method, params } = request
  if (method === 'echo') {
    return { echoed: params }
  }
  if (method === 'roundTrip') {
    const reply = await snap.request({
      method: 'snap_manageState',
      params: { operation: 'update', newState: { seen: params } },
    })
    return { fromBrowser: reply }
  }
  throw new Error('unknown method: ' + method)
}
