#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std::chrono_literals;

static void requestComplete(Request *request)
{
    if (request->status() == Request::RequestCancelled)
    {
        return;
    }

    const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();
    for (auto bufferPair : buffers) {
        FrameBuffer *buffer = bufferPair.second;

        const libcamera::FrameBuffer::Plane &plane = buffer->planes().front();
        void *frameData = mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);

        std::string photoFilename = "captured_photo_final.ppm";
        std::ofstream photoFile(photoFilename, std::ios::out | std::ios::binary);

        photoFile << "P6\n";
        photoFile << "1920 1080" << "\n";
        photoFile << "255\n";
        photoFile.write(reinterpret_cast<const char *>(frameData), plane.length);
        photoFile.close();
    }
}

int main()
{
    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    cm->start();
    static std::shared_ptr<Camera> camera = cm->cameras()[0];
    camera->acquire();

    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration( { StreamRole::Raw } );
    StreamConfiguration &streamConfig = config->at(0);
    streamConfig.pixelFormat = libcamera::formats::BGR888;
    streamConfig.size = libcamera::Size(1920, 1080);
    config->validate();
    camera->configure(config.get());

    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

    int ret = allocator->allocate(streamConfig.stream());
    if (ret < 0) {
        std::cerr << "Can't allocate buffers" << std::endl;
        return -ENOMEM;
    }

    Stream *stream = streamConfig.stream();
    const std::unique_ptr<FrameBuffer> &buffer = allocator->buffers(stream)[0];

    std::unique_ptr<Request> request = camera->createRequest();
    if (!request)
    {
        std::cerr << "Can't create request" << std::endl;
        return -ENOMEM;
    }

    ret = request->addBuffer(stream, buffer.get());
    if (ret < 0)
    {
        std::cerr << "Can't set buffer for request"
              << std::endl;
        return ret;
    }

    camera->requestCompleted.connect(requestComplete);

    camera->start();
    ControlList &reqcon = request.get()->controls();
    reqcon.set(controls::ContrastLoHistogram, 0.0);
	// reqcon.set(controls::ContrastLoLevel, 0.1);
	// reqcon.set(controls::ContrastLoMax, 20000);
	// reqcon.set(controls::ContrastHiLevel, 1.0);
	// reqcon.set(controls::ContrastHiHistogram, 0.9);
	// reqcon.set(controls::ContrastHiMax, 10000);
    camera->queueRequest(request.get());
    std::this_thread::sleep_for(1000ms);
    camera->stop();
    allocator->free(stream);
    delete allocator;
    camera->release();
    camera.reset();
    cm->stop();

    return 0;
}
