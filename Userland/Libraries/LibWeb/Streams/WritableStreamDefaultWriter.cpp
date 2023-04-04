/*
 * Copyright (c) 2023, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/PromiseCapability.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Bindings/WritableStreamDefaultWriterPrototype.h>
#include <LibWeb/Streams/AbstractOperations.h>
#include <LibWeb/Streams/WritableStream.h>
#include <LibWeb/Streams/WritableStreamDefaultWriter.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::Streams {

WebIDL::ExceptionOr<JS::NonnullGCPtr<WritableStreamDefaultWriter>> WritableStreamDefaultWriter::construct_impl(JS::Realm& realm, JS::NonnullGCPtr<WritableStream> stream)
{
    auto writer = MUST_OR_THROW_OOM(realm.heap().allocate<WritableStreamDefaultWriter>(realm, realm));

    // 1. Perform ? SetUpWritableStreamDefaultWriter(this, stream).
    TRY(set_up_writable_stream_default_writer(*writer, stream));

    return writer;
}

// https://streams.spec.whatwg.org/#default-writer-closed
JS::GCPtr<JS::Object> WritableStreamDefaultWriter::closed()
{
    // 1. Return this.[[closedPromise]].
    return m_closed_promise->promise();
}

// https://streams.spec.whatwg.org/#default-writer-desired-size
WebIDL::ExceptionOr<Optional<double>> WritableStreamDefaultWriter::desired_size() const
{
    // 1. If this.[[stream]] is undefined, throw a TypeError exception.
    if (!m_stream)
        return WebIDL::SimpleException { WebIDL::SimpleExceptionType::TypeError, "Cannot get desired size of writer that has no locked stream"sv };

    // 2. Return ! WritableStreamDefaultWriterGetDesiredSize(this).
    return writable_stream_default_writer_get_desired_size(*this);
}

// https://streams.spec.whatwg.org/#default-writer-ready
JS::GCPtr<JS::Object> WritableStreamDefaultWriter::ready()
{
    // 1. Return this.[[readyPromise]].
    return m_ready_promise->promise();
}

WritableStreamDefaultWriter::WritableStreamDefaultWriter(JS::Realm& realm)
    : Bindings::PlatformObject(realm)
{
}

JS::ThrowCompletionOr<void> WritableStreamDefaultWriter::initialize(JS::Realm& realm)
{
    MUST_OR_THROW_OOM(Base::initialize(realm));
    set_prototype(&Bindings::ensure_web_prototype<Bindings::WritableStreamDefaultWriterPrototype>(realm, "WritableStreamDefaultWriter"));

    return {};
}

void WritableStreamDefaultWriter::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_closed_promise);
    visitor.visit(m_ready_promise);
    visitor.visit(m_stream);
}

}