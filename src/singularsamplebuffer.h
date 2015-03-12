#ifndef SINGULARSAMPLEBUFFER_H
#define SINGULARSAMPLEBUFFER_H

#include "samplebuffer.h"

// A singular FIFO/LIFO sample buffer with fixed capacity and range
// checking.
//
// Common use case: Consume all buffered samples before the capacity
// is exhausted.
//
// This class is not thread-safe and not intended to be used from multiple
// threads!
class SingularSampleBuffer {
    Q_DISABLE_COPY(SingularSampleBuffer);

public:
    SingularSampleBuffer();
    explicit SingularSampleBuffer(SINT capacity);
    virtual ~SingularSampleBuffer() {}

    // The initial/total capacity of the buffer.
    SINT getCapacity() const {
        return m_primaryBuffer.size();
    }

    // The capacity at the tail that is immediately available
    // without trimming the buffer.
    SINT getTailCapacity() const {
        return getCapacity() - m_tailOffset;
    }

    bool isEmpty() const {
        return m_tailOffset <= m_headOffset;
    }

    // Returns the current size of the buffer, i.e. the number of
    // buffered samples that have been written and are available
    // for reading.
    SINT getSize() const {
        return m_tailOffset - m_headOffset;
    }

    // Discards all buffered samples and resets the buffer to its
    // initial state.
    void reset();

    // Discards all buffered samples and resets the buffer to its
    // initial state with a new capacity
    virtual void resetCapacity(SINT capacity);

    // Reserves space at the buffer's tail for writing samples. The internal
    // buffer is trimmed if necessary to provide a continuous memory region.
    //
    // Returns a pointer to the continuous memory region and the actual number
    // of samples that have been reserved. The maximum growth is limited by
    // getTailCapacity() and might be increased by calling trim().
    std::pair<CSAMPLE*, SINT> growTail(SINT size);

    // Shrinks the buffer from the tail for reading buffered samples.
    //
    // Returns a pointer to the continuous memory region and the actual
    // number of buffered samples that have been dropped. The pointer is
    // valid for reading as long as no modifying member function is called!
    std::pair<const CSAMPLE*, SINT> shrinkTail(SINT size);

    // Shrinks the buffer from the head for reading buffered samples.
    //
    // Returns a pointer to the continuous memory region and the actual
    // number of buffered samples that have been dropped. The pointer is
    // valid for reading as long as no modifying member function is called!
    std::pair<const CSAMPLE*, SINT> shrinkHead(SINT size);

protected:
    SampleBuffer m_primaryBuffer;
    SINT m_headOffset;
    SINT m_tailOffset;
};

#define DEBUG_ASSERT_CLASS_INVARIANT_SingularSampleBuffer \
    DEBUG_ASSERT(0 <= m_headOffset); \
    DEBUG_ASSERT(m_headOffset <= m_tailOffset); \
    DEBUG_ASSERT(m_tailOffset <= m_primaryBuffer.size()); \
    DEBUG_ASSERT(!isEmpty() || (0 == m_headOffset)); \
    DEBUG_ASSERT(!isEmpty() || (0 == m_tailOffset))

#endif // SINGULARSAMPLEBUFFER_H
