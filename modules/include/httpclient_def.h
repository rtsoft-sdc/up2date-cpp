#pragma once
namespace httpclient {
    enum class Error {
        Success = 0,
        Unknown,
        Connection,
        BindIPAddress,
        Read,
        Write,
        ExceedRedirectCount,
        Canceled,
        SSLConnection,
        SSLLoadingCerts,
        SSLServerVerification,
        UnsupportedMultipartBoundaryChars,
        Compression,
    };
}