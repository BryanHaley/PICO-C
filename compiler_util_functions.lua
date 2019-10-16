-- _PCC_SHALLOW_COPY
function _PCC_SHALLOW_COPY(_PCC_t)
    local _PCC_t2 = {}
    for _PCC_k,_PCC_v in pairs(_PCC_t) do
        _PCC_t2[_PCC_k] = _PCC_v
    end
    return _PCC_t2
end
-- END

-- _PCC_SHALLOW_COPY
function _PCC_NEW_MULTI_DIM_ARRAY(_PCC_dim)
    local MT = {};
    for _PCC_i=0, _PCC_dim do
        MT[_PCC_i] = {__index = function(_PCC_t, _PCC_k)
            if _PCC_i < _PCC_dim then
                _PCC_t[_PCC_k] = setmetatable({}, MT[_PCC_i+1])
                return _PCC_t[_PCC_k];
            end
        end}
    end

    return setmetatable({}, MT[0]);
end
-- END