const button = document.createElement("button")
button.innerText = "Screen Share"
button.style.fontSize = `${90/(button.innerText.length)}vw`
document.body.appendChild(button)
button.addEventListener("click", () => {
    navigator.mediaDevices.getDisplayMedia({
        video: true,
    }).then(stream => {
        const encoder = new VideoEncoder({
            output(chunk, meta) {
                const buf = new Uint8Array(chunk.byteLength)
                chunk.copyTo(buf)
                console.log(chunk, buf, meta)
                if (chunk.type === "key") {
                    const blob = new Blob([buf])
                    const url = URL.createObjectURL(blob)
                    const a = document.createElement("a")
                    a.href = url
                    a.download = "hevc.bin"
                    a.click()
                }
                if (meta?.decoderConfig?.description != null) {
                    const blob = new Blob([meta.decoderConfig.description])
                    const url = URL.createObjectURL(blob)
                    const a = document.createElement("a")
                    a.href = url
                    a.download = "hevc.desc.bin"
                    a.click()
                }
            },
            error: console.error,
        })
        encoder.configure({
            codec: "vp09.00.10.08",
            width: 2732,
            height: 2048,
        })
        const video = document.createElement("video")
        const onFrame = () => {
            const canvas = document.createElement("canvas")
            const context = canvas.getContext("2d")
            if (context == null) throw new Error("context fail")
            canvas.width = video.videoWidth
            canvas.height = video.videoHeight
            context.drawImage(video, 0, 0)
            const frame = new VideoFrame(canvas, {
                timestamp: performance.now(),
            })
            encoder.encode(frame)
            frame.close()
            // video.requestVideoFrameCallback(onFrame)
        }
        video.srcObject = stream
        video.autoplay = true
        video.requestVideoFrameCallback(onFrame)
        document.body.appendChild(video)
    })
})