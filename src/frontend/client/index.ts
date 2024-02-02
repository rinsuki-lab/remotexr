import { vs as vsSource, fs as fsSource } from "./shader"

const hevcBin = await fetch("/vp9.bin").then(r => r.arrayBuffer())
const hevcDescBin = await fetch("/vp9.desc.bin").then(r => r.arrayBuffer())

let lastFrame: VideoFrame | null = null

const decoder = new VideoDecoder({
    output(frame) {
        if (lastFrame != null) lastFrame.close()
        lastFrame = frame
    },
    error(e) {
        console.error(e)
    },
})
console.log("hi")
try {
    decoder.configure({
        codec: "vp09.00.10.08",
        codedWidth: 2732,
        codedHeight: 2048,
        colorSpace: {
            primaries: "bt709",
            transfer: "bt709",
            matrix: "bt709",
            fullRange: false,
        },
        description: hevcDescBin,
    })
} catch(e) {
    console.error(e)
    alert("webcodecs failed")
}
decoder.decode(new EncodedVideoChunk({
    data: hevcBin,
    timestamp: 0,
    type: "key",
}))
console.log("webcodecs init done")

if (navigator.xr == null) {
    alert("your browser is not supporting WebXR :sob:")
    throw new Error("not supporting webxr")
}

if (!await navigator.xr.isSessionSupported("immersive-vr")) {
    alert("your browser is not supporting immersive VR mode of WebXR :sob:")
    throw new Error("not supporting immersive-vr")
}

const button = document.createElement("button")
button.innerText = "LINK START"
button.style.fontSize = `${90/(button.innerText.length)}vw`
button.addEventListener("click", () => {
    const canvas = document.createElement("canvas")
    const gl = canvas.getContext("webgl2")
    if (gl == null) {
        alert("failed to webgl")
        return
    }
    navigator.xr?.requestSession("immersive-vr", {
    }).then(async session => {
        await gl?.makeXRCompatible()
        const glLayer = new XRWebGLLayer(session, gl, {
            framebufferScaleFactor: 1.0,
        })
        const referenceSpace = await session.requestReferenceSpace("local")
        await session.updateRenderState({
            baseLayer: glLayer,
        })
        const vs = gl.createShader(gl.VERTEX_SHADER)
        if (vs == null) throw new Error("vs fail")
        gl.shaderSource(vs, vsSource)
        gl.compileShader(vs)
        if (!gl.getShaderParameter(vs, gl.COMPILE_STATUS)) {
            throw new Error(gl.getShaderInfoLog(vs) ?? "")
        }
        const fs = gl.createShader(gl.FRAGMENT_SHADER)
        if (fs == null) throw new Error("fs fail")
        gl.shaderSource(fs, fsSource)
        gl.compileShader(fs)
        if (!gl.getShaderParameter(fs, gl.COMPILE_STATUS)) {
            throw new Error(gl.getShaderInfoLog(fs) ?? "")
        }

        const program = gl.createProgram()
        if (program == null) throw new Error("program fail")
        gl.attachShader(program, vs)
        gl.attachShader(program, fs)
        gl.linkProgram(program)
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            throw new Error(gl.getProgramInfoLog(program) ?? "")
        }
        gl.useProgram(program)

        const texCoord = gl.getAttribLocation(program, "a_texCoord")
        gl.enableVertexAttribArray(texCoord)
        const uvPos = new Float32Array([0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0])
        gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer())
        gl.bufferData(gl.ARRAY_BUFFER, uvPos, gl.STATIC_DRAW)
        gl.vertexAttribPointer(texCoord, 2, gl.FLOAT, false, 0, 0)
        gl.activeTexture(gl.TEXTURE0)
        gl.bindTexture(gl.TEXTURE_2D, gl.createTexture())
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
        gl.uniform1i(gl.getUniformLocation(program, "sampler0"), 0)
        let texture = new Uint8Array(0)

        const raf: XRFrameRequestCallback = (time, frame) => {
            gl.bindFramebuffer(gl.FRAMEBUFFER, glLayer.framebuffer)
            const pose = frame.getViewerPose(referenceSpace)
            if (pose == null) return
            for (const view of pose.views) {
                const viewport = glLayer.getViewport(view)
                if (viewport == null) continue
                gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height)
                const newLength = viewport.width * viewport.height
                if (newLength !== texture.length) {
                    texture = new Uint8Array(newLength)
                    for (let y=0; y<viewport.height; y++) for (let x=0; x<viewport.width; x++) {
                        const i = (y * viewport.width) + x
                        texture[y * viewport.width + x] = ((x & 1) + (y & 1)) & 1 ? 0 : 255
                    }
                }
                gl.activeTexture(gl.TEXTURE0)
                // gl.texImage2D(gl.TEXTURE_2D, 0, gl.LUMINANCE, viewport.width, viewport.height, 0, gl.LUMINANCE, gl.UNSIGNED_BYTE, texture)
                if (lastFrame != null) {
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, lastFrame.codedWidth, lastFrame.codedHeight, 0, gl.RGBA, gl.UNSIGNED_BYTE, null)
                    gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, gl.RGBA, gl.UNSIGNED_BYTE, lastFrame)
                }
                gl.drawArrays(gl.TRIANGLE_FAN, 0, 4)
                gl.flush()
            }
            session.requestAnimationFrame(raf)
        }
        session.requestAnimationFrame(raf)
    }).catch(e => {
        console.error(e)
        alert("requestSession failed: " + `${e} (${e.message})`)
    })
})
document.body.appendChild(button)

export {}