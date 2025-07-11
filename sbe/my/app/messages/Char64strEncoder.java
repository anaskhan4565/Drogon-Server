/* Generated SBE (Simple Binary Encoding) message codec. */
package my.app.messages;

import org.agrona.MutableDirectBuffer;

@SuppressWarnings("all")
public final class Char64strEncoder
{
    public static final int SCHEMA_ID = 1;
    public static final int SCHEMA_VERSION = 1;
    public static final int ENCODED_LENGTH = 64;
    public static final java.nio.ByteOrder BYTE_ORDER = java.nio.ByteOrder.LITTLE_ENDIAN;

    private int offset;
    private MutableDirectBuffer buffer;

    public Char64strEncoder wrap(final MutableDirectBuffer buffer, final int offset)
    {
        if (buffer != this.buffer)
        {
            this.buffer = buffer;
        }
        this.offset = offset;

        return this;
    }

    public MutableDirectBuffer buffer()
    {
        return buffer;
    }

    public int offset()
    {
        return offset;
    }

    public int encodedLength()
    {
        return ENCODED_LENGTH;
    }

    public int sbeSchemaId()
    {
        return SCHEMA_ID;
    }

    public int sbeSchemaVersion()
    {
        return SCHEMA_VERSION;
    }

    public static int charsEncodingOffset()
    {
        return 0;
    }

    public static int charsEncodingLength()
    {
        return 64;
    }

    public static byte charsNullValue()
    {
        return (byte)0;
    }

    public static byte charsMinValue()
    {
        return (byte)32;
    }

    public static byte charsMaxValue()
    {
        return (byte)126;
    }

    public static int charsLength()
    {
        return 64;
    }


    public Char64strEncoder chars(final int index, final byte value)
    {
        if (index < 0 || index >= 64)
        {
            throw new IndexOutOfBoundsException("index out of range: index=" + index);
        }

        final int pos = offset + 0 + (index * 1);
        buffer.putByte(pos, value);

        return this;
    }

    public static String charsCharacterEncoding()
    {
        return java.nio.charset.StandardCharsets.US_ASCII.name();
    }

    public Char64strEncoder putChars(final byte[] src, final int srcOffset)
    {
        final int length = 64;
        if (srcOffset < 0 || srcOffset > (src.length - length))
        {
            throw new IndexOutOfBoundsException("Copy will go out of range: offset=" + srcOffset);
        }

        buffer.putBytes(offset + 0, src, srcOffset, length);

        return this;
    }

    public Char64strEncoder chars(final String src)
    {
        final int length = 64;
        final int srcLength = null == src ? 0 : src.length();
        if (srcLength > length)
        {
            throw new IndexOutOfBoundsException("String too large for copy: byte length=" + srcLength);
        }

        buffer.putStringWithoutLengthAscii(offset + 0, src);

        for (int start = srcLength; start < length; ++start)
        {
            buffer.putByte(offset + 0 + start, (byte)0);
        }

        return this;
    }

    public Char64strEncoder chars(final CharSequence src)
    {
        final int length = 64;
        final int srcLength = null == src ? 0 : src.length();
        if (srcLength > length)
        {
            throw new IndexOutOfBoundsException("CharSequence too large for copy: byte length=" + srcLength);
        }

        buffer.putStringWithoutLengthAscii(offset + 0, src);

        for (int start = srcLength; start < length; ++start)
        {
            buffer.putByte(offset + 0 + start, (byte)0);
        }

        return this;
    }

    public String toString()
    {
        if (null == buffer)
        {
            return "";
        }

        return appendTo(new StringBuilder()).toString();
    }

    public StringBuilder appendTo(final StringBuilder builder)
    {
        if (null == buffer)
        {
            return builder;
        }

        final Char64strDecoder decoder = new Char64strDecoder();
        decoder.wrap(buffer, offset);

        return decoder.appendTo(builder);
    }
}
