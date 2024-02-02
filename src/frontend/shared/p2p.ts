export const peerConn = new RTCPeerConnection({
    iceServers: [{
        urls: ["stun:stun.l.google.com:19302"]
    }]
})
export const dataChannel = peerConn.createDataChannel("data", {
    negotiated: true,
    id: 1,
})
export function startConnection() {
    
}
