/* Generated SBE (Simple Binary Encoding) message codec. */
package my.app.messages;

import org.agrona.DirectBuffer;

@SuppressWarnings("all")
public final class Char64strDecoder
{
    public static final int SCHEMA_ID = 1;
    public static final int SCHEMA_VERSION = 1;
    public static final int ENCODED_LENGTH = 64;
    public static final java.nio.ByteOrder BYTE_ORDER = java.nio.ByteOrder.LITTLE_ENDIAN;

    private int offset;
    private DirectBuffer buffer;

    public Char64strDecoder wrap(final DirectBuffer buffer, final int offset)
    {
        if (buffer != this.buffer)
        {
            this.buffer = buffer;
        }
        this.offset = offset;

        return this;
    }

    public DirectBuffer buffer()
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

    public static int charsSinceVersion()
    {
        return 0;
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


    public byte chars(final int index)
    {
        if (index < 0 || index >= 64)
        {
            throw new IndexOutOfBoundsException("index out of range: index=" + index);
        }

        final int pos = offset + 0 + (index * 1);

        return buffer.getByte(pos);
    }


    public static String charsCharacterEncoding()
    {
        return java.nio.charset.StandardCharsets.US_ASCII.name();
    }

    public int getChars(final byte[] dst, final int dstOffset)
    {
        final int length = 64;
        if (dstOffset < 0 || dstOffset > (dst.length - length))
        {
            throw new IndexOutOfBoundsException("Copy will go out of range: offset=" + dstOffset);
        }

        buffer.getBytes(offset + 0, dst, dstOffset, length);

        return length;
    }

    public String chars()
    {
        final byte[] dst = new byte[64];
        buffer.getBytes(offset + 0, dst, 0, 64);

        int end = 0;
        for (; end < 64 && dst[end] != 0; ++end);

        return new String(dst, 0, end, java.nio.charset.StandardCharsets.US_ASCII);
    }


    public int getChars(final Appendable value)
    {
        for (int i = 0; i < 64; ++i)
        {
            final int c = buffer.getByte(offset + 0 + i) & 0xFF;
            if (c == 0)
            {
                return i;
            }

            try
            {
                value.append(c > 127 ? '?' : (char)c);
            }
            catch (final java.io.IOException ex)
            {
                throw new java.io.UncheckedIOException(ex);
            }
        }

        return 64;
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

        builder.append('(');
        builder.append("chars=");
        for (int i = 0; i < charsLength() && chars(i) > 0; i++)
        {
            builder.append((char)chars(i));
        }
        builder.append(')');

        return builder;
    }
}
