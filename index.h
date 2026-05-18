import { useState, useRef, useEffect, useCallback } from "react";

/* ── Load html2canvas from CDN ─────────────────────────────────────────────── */
let html2canvasLoaded = false;
function loadHtml2Canvas() {
  return new Promise((resolve) => {
    if (window.html2canvas) { resolve(window.html2canvas); return; }
    if (html2canvasLoaded) {
      const wait = setInterval(() => {
        if (window.html2canvas) { clearInterval(wait); resolve(window.html2canvas); }
      }, 100);
      return;
    }
    html2canvasLoaded = true;
    const s = document.createElement("script");
    s.src = "https://cdnjs.cloudflare.com/ajax/libs/html2canvas/1.4.1/html2canvas.min.js";
    s.onload = () => resolve(window.html2canvas);
    document.head.appendChild(s);
  });
}

/* ── Constants ─────────────────────────────────────────────────────────────── */
const C = {
  red:"#C8102E", black:"#111111", green:"#007A3D",
  gold:"#D4AF37", goldLight:"#F5D76E", goldDark:"#9A7B1C",
};

function genMemberID() {
  const yr = new Date().getFullYear();
  return `PPP-PS101-${yr}-${Math.floor(100000 + Math.random() * 900000)}`;
}
function todayEn() {
  return new Date().toLocaleDateString("en-GB",{year:"numeric",month:"short",day:"2-digit"});
}
function todayUrdu() {
  return new Date().toLocaleDateString("ur-PK",{year:"numeric",month:"long",day:"numeric"});
}

const ROLES=[
  "سوشل میڈیا ایکٹیوسٹ","ورکر / کارکن","یوتھ لیڈر","وومن ونگ",
  "ڈاکیا / مقامی رہنما","یونٹ صدر","یونٹ سیکرٹری","پولنگ ایجنٹ",
  "میڈیا کوآرڈینیٹر","جنرل سیکرٹری","دیگر"
];
const AREAS=[
  "سہراب گوتھ","صفورا چورنگی","نارتھ کراچی","نیو کراچی","گلستان جوہر",
  "گلشن اقبال","پی آئی بی کالونی","اورنگی ٹاؤن","دیگر علاقہ"
];
const PARTIES=["پاکستان مسلم لیگ (ن)","پاکستان تحریک انصاف","ایم کیو ایم","جماعت اسلامی","آزاد / کوئی نہیں","دیگر"];
const TABS=["📋 ممبرشپ فارم","👥 تمام ممبران","🔒 ایڈمن"];
const ADMIN_PASS="ppp786";

/* ── PPP Flag SVG ───────────────────────────────────────────────────────────── */
function PPPFlagSVG({ w=120, h=80, round=6 }) {
  return (
    <svg width={w} height={h} viewBox="0 0 120 80" xmlns="http://www.w3.org/2000/svg">
      <defs>
        <clipPath id="fc2"><rect x="0" y="0" width="120" height="80" rx={round}/></clipPath>
        <radialGradient id="rg2" cx="30%" cy="40%"><stop offset="0%" stopColor="#FF2040"/><stop offset="100%" stopColor="#8B0000"/></radialGradient>
        <radialGradient id="bg3" cx="50%" cy="40%"><stop offset="0%" stopColor="#2a2a2a"/><stop offset="100%" stopColor="#060606"/></radialGradient>
        <radialGradient id="gg2" cx="70%" cy="40%"><stop offset="0%" stopColor="#00CC55"/><stop offset="100%" stopColor="#004D1A"/></radialGradient>
      </defs>
      <g clipPath="url(#fc2)">
        <rect x="0"  y="0" width="40" height="80" fill="url(#rg2)"/>
        <rect x="40" y="0" width="40" height="80" fill="url(#bg3)"/>
        <rect x="80" y="0" width="40" height="80" fill="url(#gg2)"/>
        <circle cx="57" cy="40" r="16" fill="white" opacity="0.96"/>
        <circle cx="63" cy="36" r="12.5" fill="#1a1a1a"/>
        <polygon points="75,24 77.1,30.5 84,30.5 78.5,34.5 80.6,41 75,37 69.4,41 71.5,34.5 66,30.5 72.9,30.5" fill="white" opacity="0.96"/>
      </g>
    </svg>
  );
}

/* ── QR Code component ──────────────────────────────────────────────────────── */
function QRCode({ data, size=76 }) {
  const url = `https://api.qrserver.com/v1/create-qr-code/?size=${size*2}x${size*2}&data=${encodeURIComponent(data)}&color=D4AF37&bgcolor=0d0d0d&margin=6&format=svg`;
  return <img src={url} width={size} height={size} style={{borderRadius:"6px",border:"2px solid rgba(212,175,55,0.5)",display:"block"}} alt="QR"/>;
}

/* ── SIGNATURE PAD ──────────────────────────────────────────────────────────── */
function SignaturePad({ onSave, existingSig }) {
  const canvasRef = useRef();
  const [drawing, setDrawing] = useState(false);
  const [hasSig, setHasSig] = useState(!!existingSig);
  const lastPos = useRef(null);

  function getPos(e, canvas) {
    const rect = canvas.getBoundingClientRect();
    const src = e.touches ? e.touches[0] : e;
    return { x: src.clientX - rect.left, y: src.clientY - rect.top };
  }

  function startDraw(e) {
    e.preventDefault();
    const canvas = canvasRef.current;
    const ctx = canvas.getContext("2d");
    ctx.strokeStyle = "#1a1a1a";
    ctx.lineWidth = 2;
    ctx.lineCap = "round";
    ctx.lineJoin = "round";
    lastPos.current = getPos(e, canvas);
    setDrawing(true);
  }

  function draw(e) {
    if (!drawing) return;
    e.preventDefault();
    const canvas = canvasRef.current;
    const ctx = canvas.getContext("2d");
    const pos = getPos(e, canvas);
    ctx.beginPath();
    ctx.moveTo(lastPos.current.x, lastPos.current.y);
    ctx.lineTo(pos.x, pos.y);
    ctx.stroke();
    lastPos.current = pos;
    setHasSig(true);
  }

  function endDraw(e) {
    e.preventDefault();
    setDrawing(false);
  }

  function clearSig() {
    const canvas = canvasRef.current;
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    setHasSig(false);
    onSave(null);
  }

  function saveSig() {
    const canvas = canvasRef.current;
    const dataURL = canvas.toDataURL("image/png");
    onSave(dataURL);
  }

  useEffect(() => {
    if (existingSig && canvasRef.current) {
      const canvas = canvasRef.current;
      const ctx = canvas.getContext("2d");
      const img = new Image();
      img.onload = () => ctx.drawImage(img, 0, 0);
      img.src = existingSig;
    }
  }, []);

  return (
    <div style={{textAlign:"center"}}>
      <div style={{fontSize:"11px",color:"#666",marginBottom:"6px",direction:"rtl"}}>
        ✍️ جنرل سیکرٹری — دستخط کریں
      </div>
      <div style={{
        border:"1.5px solid #aaa",borderRadius:"8px",background:"#fffdf5",
        display:"inline-block",position:"relative"
      }}>
        <canvas
          ref={canvasRef}
          width={220} height={70}
          style={{display:"block",borderRadius:"8px",touchAction:"none",cursor:"crosshair"}}
          onMouseDown={startDraw} onMouseMove={draw} onMouseUp={endDraw} onMouseLeave={endDraw}
          onTouchStart={startDraw} onTouchMove={draw} onTouchEnd={endDraw}
        />
        {!hasSig && (
          <div style={{
            position:"absolute",inset:0,display:"flex",alignItems:"center",
            justifyContent:"center",pointerEvents:"none",
            color:"#ccc",fontSize:"11px",letterSpacing:"1px"
          }}>یہاں دستخط کریں</div>
        )}
      </div>
      <div style={{display:"flex",gap:"8px",justifyContent:"center",marginTop:"6px"}}>
        <button onClick={clearSig} style={{
          background:"#fee",color:C.red,border:`1px solid ${C.red}`,
          padding:"4px 12px",borderRadius:"6px",cursor:"pointer",fontSize:"11px"
        }}>🗑 صاف</button>
        {hasSig && (
          <button onClick={saveSig} style={{
            background:C.green,color:"white",border:"none",
            padding:"4px 12px",borderRadius:"6px",cursor:"pointer",fontSize:"11px"
          }}>✅ دستخط محفوظ</button>
        )}
      </div>
    </div>
  );
}

/* ── FULL CARD FRONT (for rendering/capture) ────────────────────────────────── */
function CardFront({ member, signature }) {
  const qrData = [
    `PPP-PS101`,
    `ID:${member.memberID}`,
    `Name:${member.name}`,
    `Role:${member.role}`,
    `Area:${member.area}`,
    `Mobile:${member.mobile}`,
    member.twitter?`X:${member.twitter}`:"",
    member.facebook?`FB:${member.facebook}`:"",
    member.instagram?`IG:${member.instagram}`:"",
    member.youtube?`YT:${member.youtube}`:"",
    `Since:${member.joinedPPP||"N/A"}`,
    `Issued:${todayEn()}`,
  ].filter(Boolean).join("|");

  return (
    <div id="ppp-card-front" style={{
      width:"460px",
      background:"linear-gradient(145deg,#0a0a0a 0%,#161616 35%,#0e0e0e 70%,#131313 100%)",
      borderRadius:"16px",overflow:"hidden",
      fontFamily:"Georgia, 'Times New Roman', serif",direction:"ltr",
      border:"2px solid rgba(212,175,55,0.5)",
      boxShadow:"0 0 0 1px rgba(212,175,55,0.1),0 20px 60px rgba(0,0,0,0.8)",
    }}>
      {/* Top flag stripe */}
      <div style={{display:"flex",height:"9px"}}>
        <div style={{flex:1,background:"linear-gradient(to right,#8B0000,#C8102E,#FF2040)"}}/>
        <div style={{flex:1,background:"linear-gradient(to right,#050505,#1c1c1c,#050505)"}}/>
        <div style={{flex:1,background:"linear-gradient(to right,#004D1A,#007A3D,#00CC55)"}}/>
      </div>
      <div style={{height:"1.5px",background:"linear-gradient(to right,transparent,#9A7B1C,#F5D76E,#9A7B1C,transparent)"}}/>

      {/* Header */}
      <div style={{padding:"12px 18px 10px",background:"linear-gradient(to bottom,rgba(212,175,55,0.06),transparent)",
        display:"flex",alignItems:"center",gap:"14px",borderBottom:"1px solid rgba(212,175,55,0.1)"}}>
        <div style={{flexShrink:0,filter:"drop-shadow(0 4px 12px rgba(0,0,0,0.9))"}}>
          <PPPFlagSVG w={104} h={69} round={8}/>
        </div>
        <div style={{flex:1,textAlign:"center"}}>
          <div style={{fontSize:"6px",letterSpacing:"4px",color:"rgba(212,175,55,0.55)",textTransform:"uppercase",marginBottom:"4px"}}>
            اقتدار عوام کا • PEOPLE'S POWER
          </div>
          <div style={{fontSize:"15px",fontWeight:"bold",letterSpacing:"0.8px",
            background:"linear-gradient(to right,#9A7B1C,#F5D76E,#D4AF37,#F5D76E,#9A7B1C)",
            WebkitBackgroundClip:"text",WebkitTextFillColor:"transparent",textTransform:"uppercase"}}>
            Pakistan Peoples Party
          </div>
          <div style={{fontSize:"8.5px",color:"rgba(212,175,55,0.45)",letterSpacing:"2px",marginTop:"2px"}}>پاکستان پیپلز پارٹی</div>
          <div style={{display:"flex",alignItems:"center",gap:"5px",justifyContent:"center",marginTop:"6px"}}>
            <div style={{flex:1,height:"1px",background:"linear-gradient(to right,transparent,rgba(200,16,46,0.6))"}}/>
            <div style={{fontSize:"8px",color:"#C8102E",fontWeight:"bold",letterSpacing:"1px",whiteSpace:"nowrap"}}>
              PS-101 · DISTRICT EAST · KARACHI
            </div>
            <div style={{flex:1,height:"1px",background:"linear-gradient(to left,transparent,rgba(200,16,46,0.6))"}}/>
          </div>
          <div style={{fontSize:"6.5px",color:"rgba(255,255,255,0.25)",letterSpacing:"2px",marginTop:"4px",textTransform:"uppercase"}}>
            Social Media Wing — Official Membership Card
          </div>
        </div>
        <div style={{flexShrink:0,filter:"drop-shadow(0 4px 12px rgba(0,0,0,0.9))"}}>
          <PPPFlagSVG w={104} h={69} round={8}/>
        </div>
      </div>

      {/* Gold line */}
      <div style={{height:"1px",margin:"0 18px",background:"linear-gradient(to right,transparent,rgba(212,175,55,0.6),rgba(245,215,110,0.8),rgba(212,175,55,0.6),transparent)"}}/>

      {/* Body */}
      <div style={{padding:"14px 18px 10px",position:"relative"}}>
        <div style={{position:"absolute",inset:0,display:"flex",alignItems:"center",justifyContent:"center",
          pointerEvents:"none",fontSize:"80px",color:"rgba(212,175,55,0.03)",fontWeight:"bold",
          letterSpacing:"-5px",overflow:"hidden",userSelect:"none"}}>PPP PS-101</div>

        <div style={{display:"flex",gap:"14px",alignItems:"flex-start",position:"relative"}}>
          {/* Photo */}
          <div style={{flexShrink:0}}>
            <div style={{padding:"3px",borderRadius:"11px",
              background:"linear-gradient(135deg,#F5D76E,#9A7B1C,#D4AF37,#9A7B1C,#F5D76E)",
              boxShadow:"0 0 20px rgba(212,175,55,0.25)"}}>
              <div style={{padding:"2px",borderRadius:"9px",background:"#0d0d0d"}}>
                <div style={{width:"88px",height:"100px",borderRadius:"8px",overflow:"hidden",
                  background:"linear-gradient(135deg,#1a1a1a,#2a2a2a)",
                  display:"flex",alignItems:"center",justifyContent:"center"}}>
                  {member.photo
                    ?<img src={member.photo} style={{width:"100%",height:"100%",objectFit:"cover"}} alt="member"/>
                    :<div style={{textAlign:"center",color:"rgba(212,175,55,0.3)"}}>
                      <div style={{fontSize:"28px"}}>👤</div>
                      <div style={{fontSize:"7px",marginTop:"3px",letterSpacing:"1px"}}>PHOTO</div>
                    </div>
                  }
                </div>
              </div>
            </div>
            <div style={{display:"flex",height:"3px",marginTop:"4px",borderRadius:"2px",overflow:"hidden"}}>
              <div style={{flex:1,background:"#C8102E"}}/><div style={{flex:1,background:"#111"}}/><div style={{flex:1,background:"#007A3D"}}/>
            </div>
            <div style={{textAlign:"center",marginTop:"2px",fontSize:"6px",letterSpacing:"1.5px",color:"rgba(212,175,55,0.3)",textTransform:"uppercase"}}>Photo</div>
          </div>

          {/* Info */}
          <div style={{flex:1,minWidth:0}}>
            <div style={{fontSize:"18px",fontWeight:"bold",letterSpacing:"0.3px",lineHeight:1.15,
              background:"linear-gradient(to right,#ffffff,#F5D76E,#ffffff)",
              WebkitBackgroundClip:"text",WebkitTextFillColor:"transparent"}}>{member.name}</div>
            {member.father&&<div style={{fontSize:"9px",color:"rgba(255,255,255,0.38)",marginTop:"2px"}}>S/O {member.father}</div>}
            <div style={{display:"inline-flex",alignItems:"center",gap:"5px",marginTop:"6px",
              background:"linear-gradient(135deg,rgba(200,16,46,0.9),rgba(139,0,0,0.95))",
              padding:"3px 10px 3px 7px",borderRadius:"20px",boxShadow:"0 3px 10px rgba(200,16,46,0.3)"}}>
              <div style={{width:"4px",height:"4px",borderRadius:"50%",background:"rgba(255,255,255,0.8)"}}/>
              <span style={{color:"white",fontSize:"8px",fontWeight:"bold",letterSpacing:"0.8px",textTransform:"uppercase"}}>{member.role}</span>
            </div>
            <div style={{marginTop:"8px"}}>
              <div style={{fontSize:"6px",color:"rgba(212,175,55,0.4)",letterSpacing:"2px",textTransform:"uppercase",marginBottom:"3px"}}>Member ID</div>
              <div style={{fontFamily:"'Courier New',monospace",fontSize:"10px",color:"#F5D76E",letterSpacing:"1.5px",
                background:"rgba(212,175,55,0.06)",border:"1px solid rgba(212,175,55,0.18)",
                padding:"3px 8px",borderRadius:"5px",display:"inline-block"}}>{member.memberID}</div>
            </div>
            <div style={{marginTop:"8px",display:"grid",gridTemplateColumns:"1fr 1fr",gap:"4px 8px",fontSize:"8.5px",color:"rgba(255,255,255,0.5)"}}>
              {[
                member.mobile&&["📱",member.mobile],
                member.area&&["📍",member.area],
                member.cnic&&["🪪",member.cnic],
                member.joinedPPP&&["🗓","Since "+member.joinedPPP],
              ].filter(Boolean).map(([icon,val],i)=>(
                <div key={i} style={{display:"flex",alignItems:"center",gap:"3px"}}>
                  <span style={{fontSize:"8px",flexShrink:0}}>{icon}</span>
                  <span style={{overflow:"hidden",textOverflow:"ellipsis",whiteSpace:"nowrap"}}>{val}</span>
                </div>
              ))}
            </div>
            {(member.twitter||member.facebook||member.instagram||member.youtube)&&(
              <div style={{display:"flex",gap:"3px",marginTop:"7px",flexWrap:"wrap"}}>
                {member.twitter&&<span style={{background:"rgba(0,0,0,0.8)",border:"1px solid rgba(255,255,255,0.2)",color:"white",fontSize:"7px",padding:"2px 6px",borderRadius:"10px",fontFamily:"monospace"}}>𝕏 {member.twitter}</span>}
                {member.facebook&&<span style={{background:"rgba(24,119,242,0.85)",color:"white",fontSize:"7px",padding:"2px 6px",borderRadius:"10px"}}>f {member.facebook}</span>}
                {member.instagram&&<span style={{background:"linear-gradient(45deg,#f09433,#e6683c,#dc2743,#cc2366,#bc1888)",color:"white",fontSize:"7px",padding:"2px 6px",borderRadius:"10px"}}>📸 {member.instagram}</span>}
                {member.youtube&&<span style={{background:"rgba(255,0,0,0.85)",color:"white",fontSize:"7px",padding:"2px 6px",borderRadius:"10px"}}>▶ {member.youtube}</span>}
              </div>
            )}
          </div>

          {/* QR */}
          <div style={{flexShrink:0,display:"flex",flexDirection:"column",alignItems:"center",gap:"4px"}}>
            <div style={{padding:"2px",borderRadius:"8px",background:"linear-gradient(135deg,#F5D76E,#9A7B1C,#D4AF37)",boxShadow:"0 0 12px rgba(212,175,55,0.2)"}}>
              <div style={{borderRadius:"7px",overflow:"hidden",background:"#0d0d0d",padding:"2px"}}>
                <QRCode data={qrData} size={70}/>
              </div>
            </div>
            <div style={{fontSize:"5.5px",color:"rgba(212,175,55,0.4)",letterSpacing:"1px",textAlign:"center",textTransform:"uppercase"}}>
              Scan to<br/>Verify
            </div>
          </div>
        </div>

        {/* Prev party note */}
        {member.isNew&&member.prevParty&&(
          <div style={{marginTop:"8px",padding:"5px 10px",background:"rgba(212,175,55,0.04)",
            border:"1px solid rgba(212,175,55,0.1)",borderRadius:"6px",fontSize:"7px",color:"rgba(255,255,255,0.35)"}}>
            ◈ Previously: <span style={{color:"rgba(245,215,110,0.7)"}}>{member.prevRole||"Member"}</span>
            &nbsp;at&nbsp;<span style={{color:"rgba(255,255,255,0.5)"}}>{member.prevParty}</span>
            {member.prevSince&&<span> (since {member.prevSince})</span>}
          </div>
        )}

        {/* ── SIGNATURE ROW ── */}
        <div style={{
          display:"flex",alignItems:"flex-end",justifyContent:"space-between",
          marginTop:"10px",paddingTop:"8px",
          borderTop:"1px solid rgba(212,175,55,0.15)"
        }}>
          {/* Left: Issued stamp */}
          <div style={{textAlign:"center"}}>
            <div style={{
              width:"64px",height:"28px",borderRadius:"4px",
              border:"1.5px solid rgba(200,16,46,0.5)",
              display:"flex",alignItems:"center",justifyContent:"center",
              background:"rgba(200,16,46,0.06)"
            }}>
              <div style={{textAlign:"center",color:"#C8102E",fontSize:"6px",fontWeight:"bold",lineHeight:1.4}}>
                ISSUED<br/>
                <span style={{letterSpacing:"0.3px"}}>{todayEn()}</span>
              </div>
            </div>
            <div style={{fontSize:"5.5px",color:"rgba(255,255,255,0.25)",marginTop:"2px",letterSpacing:"0.5px"}}>Date</div>
          </div>

          {/* Center: Slogan */}
          <div style={{textAlign:"center",flex:1,padding:"0 8px"}}>
            <div style={{color:"rgba(212,175,55,0.6)",fontSize:"7px",fontWeight:"bold",letterSpacing:"2px"}}>
              ☽ ROTI · KAPRA · MAKAN ★
            </div>
          </div>

          {/* Right: Signature box */}
          <div style={{textAlign:"center"}}>
            <div style={{
              width:"100px",height:"32px",borderRadius:"4px",
              border:"1px solid rgba(212,175,55,0.3)",
              background:"rgba(255,255,255,0.03)",
              display:"flex",alignItems:"center",justifyContent:"center",
              overflow:"hidden"
            }}>
              {signature
                ?<img src={signature} style={{maxWidth:"100%",maxHeight:"100%",objectFit:"contain"}}/>
                :<div style={{fontSize:"6px",color:"rgba(255,255,255,0.2)",letterSpacing:"0.5px",textAlign:"center"}}>
                  Signature
                </div>
              }
            </div>
            <div style={{fontSize:"5.5px",color:"rgba(212,175,55,0.4)",marginTop:"2px",letterSpacing:"0.5px",textAlign:"center"}}>
              General Secretary<br/>PPP PS-101
            </div>
          </div>
        </div>
      </div>

      {/* Gold line */}
      <div style={{height:"1px",margin:"0 18px",background:"linear-gradient(to right,transparent,rgba(212,175,55,0.5),rgba(245,215,110,0.8),rgba(212,175,55,0.5),transparent)"}}/>

      {/* Footer tricolor */}
      <div style={{display:"flex",alignItems:"stretch",minHeight:"28px"}}>
        <div style={{background:"linear-gradient(to right,#6B0018,#C8102E)",padding:"6px 12px",display:"flex",alignItems:"center",flexShrink:0}}>
          <div style={{color:"rgba(255,255,255,0.9)",fontSize:"6px",fontWeight:"bold",letterSpacing:"0.5px",lineHeight:1.5}}>
            PPP<br/>PS-101
          </div>
        </div>
        <div style={{flex:1,background:"linear-gradient(to right,#0a0a0a,#141414,#0a0a0a)",
          display:"flex",alignItems:"center",justifyContent:"center",gap:"5px",padding:"0 6px"}}>
          <div style={{height:"1px",flex:1,background:"linear-gradient(to right,transparent,rgba(212,175,55,0.4))"}}/>
          <div style={{textAlign:"center",color:"#D4AF37",fontSize:"7px",fontWeight:"bold",letterSpacing:"1.5px",whiteSpace:"nowrap"}}>
            KARACHI EAST · PAKISTAN
          </div>
          <div style={{height:"1px",flex:1,background:"linear-gradient(to left,transparent,rgba(212,175,55,0.4))"}}/>
        </div>
        <div style={{background:"linear-gradient(to left,#003D15,#007A3D)",padding:"6px 12px",display:"flex",alignItems:"center",flexShrink:0}}>
          <div style={{color:"rgba(255,255,255,0.9)",fontSize:"6px",fontWeight:"bold",lineHeight:1.5,textAlign:"right"}}>
            SOCIAL<br/>MEDIA
          </div>
        </div>
      </div>

      {/* Bottom flag stripe */}
      <div style={{display:"flex",height:"9px"}}>
        <div style={{flex:1,background:"linear-gradient(to right,#FF2040,#C8102E,#8B0000)"}}/>
        <div style={{flex:1,background:"linear-gradient(to right,#060606,#1a1a1a,#060606)"}}/>
        <div style={{flex:1,background:"linear-gradient(to right,#00CC55,#007A3D,#004D1A)"}}/>
      </div>

      {/* Security strip */}
      <div style={{background:"#050505",padding:"3px 18px",fontSize:"5px",color:"rgba(212,175,55,0.15)",
        letterSpacing:"1.5px",textAlign:"center",fontFamily:"'Courier New',monospace",textTransform:"uppercase"}}>
        Pakistan Peoples Party · PS-101 Social Media Wing · District East Karachi · Official Membership · Unauthorized duplication prohibited
      </div>
    </div>
  );
}

/* ── MEMBER CARD MODAL ─────────────────────────────────────────────────────── */
function MemberCard({ member, onClose, onUpdateMember }) {
  const [signature, setSignature] = useState(member.signature || null);
  const [sigMode, setSigMode] = useState(false);
  const [loading, setLoading] = useState(false);
  const [status, setStatus] = useState("");

  function handleSigSave(sig) {
    setSignature(sig);
    setSigMode(false);
    if (sig) {
      onUpdateMember({ ...member, signature: sig });
      setStatus("✅ دستخط محفوظ ہو گیا");
      setTimeout(() => setStatus(""), 2000);
    }
  }

  async function captureCard() {
    setLoading(true);
    const h2c = await loadHtml2Canvas();
    const el = document.getElementById("ppp-card-front");
    const canvas = await h2c(el, {
      scale: 2, backgroundColor: null,
      useCORS: true, allowTaint: true,
      logging: false
    });
    setLoading(false);
    return canvas;
  }

  async function downloadCard() {
    setStatus("⏳ کارڈ تیار ہو رہا ہے...");
    const canvas = await captureCard();
    const a = document.createElement("a");
    a.download = `PPP-Membership-${member.memberID}.png`;
    a.href = canvas.toDataURL("image/png");
    a.click();
    setStatus("✅ کارڈ ڈاؤنلوڈ ہو گیا!");
    setTimeout(() => setStatus(""), 3000);
  }

  async function sendToWhatsApp() {
    if (!member.mobile && !member.whatsapp) {
      setStatus("⚠ واٹس ایپ نمبر موجود نہیں");
      return;
    }
    setStatus("⏳ کارڈ تصویر بنائی جا رہی ہے...");
    const canvas = await captureCard();

    // Download image first
    const a = document.createElement("a");
    a.download = `PPP-Card-${member.memberID}.png`;
    a.href = canvas.toDataURL("image/png");
    a.click();

    // Format WhatsApp number
    let num = (member.whatsapp || member.mobile).replace(/[\s\-\(\)]/g, "");
    if (num.startsWith("0")) num = "92" + num.slice(1);

    const msg = encodeURIComponent(
      `🎉 *PPP PS-101 Membership Card*\n\n` +
      `*Name:* ${member.name}\n` +
      `*Member ID:* ${member.memberID}\n` +
      `*Role:* ${member.role}\n` +
      `*Area:* ${member.area}\n\n` +
      `تصویر ڈاؤنلوڈ ہو گئی ہے — پلاسٹک کارڈ پرنٹ کروانے کے لیے یہ تصویر پرنٹ شاپ پر لے جائیں 🇵🇰\n\n` +
      `Pakistan Peoples Party — PS-101 District East Karachi`
    );

    setTimeout(() => {
      window.open(`https://wa.me/${num}?text=${msg}`, "_blank");
      setStatus("✅ واٹس ایپ کھل رہا ہے — کارڈ تصویر بھیجیں!");
    }, 800);
  }

  function printCard() {
    const w = window.open("","_blank");
    const el = document.getElementById("ppp-card-front");
    w.document.write(`<!DOCTYPE html><html><head><title>PPP Card - ${member.name}</title>
    <style>*{margin:0;padding:0;box-sizing:border-box}
    body{background:#fff;display:flex;align-items:center;justify-content:center;
    min-height:100vh;padding:20px;font-family:Georgia,serif}
    @media print{body{padding:0}}</style></head>
    <body onload="window.print()">${el.outerHTML}</body></html>`);
    w.document.close();
  }

  return (
    <div style={{
      position:"fixed",inset:0,
      background:"radial-gradient(ellipse at 30% 50%,rgba(200,16,46,0.12) 0%,rgba(0,0,0,0.97) 70%)",
      display:"flex",alignItems:"center",justifyContent:"center",
      zIndex:1000,padding:"10px",flexDirection:"column",gap:"12px",
      overflowY:"auto"
    }}>
      {/* Card */}
      <CardFront member={member} signature={signature}/>

      {/* Status */}
      {status&&(
        <div style={{
          background:"rgba(0,0,0,0.8)",color:"white",padding:"8px 20px",
          borderRadius:"20px",fontSize:"13px",border:"1px solid rgba(255,255,255,0.2)"
        }}>{status}</div>
      )}

      {/* Signature Pad toggle */}
      {sigMode ? (
        <div style={{
          background:"white",borderRadius:"12px",padding:"16px",
          boxShadow:"0 8px 32px rgba(0,0,0,0.5)"
        }}>
          <SignaturePad onSave={handleSigSave} existingSig={signature}/>
        </div>
      ) : (
        <button onClick={()=>setSigMode(true)} style={{
          background:"rgba(212,175,55,0.15)",color:"#D4AF37",
          border:"1px solid rgba(212,175,55,0.4)",padding:"8px 18px",
          borderRadius:"8px",cursor:"pointer",fontSize:"12px",fontWeight:"bold"
        }}>
          ✍️ {signature?"دستخط تبدیل کریں":"جنرل سیکرٹری دستخط کریں"}
        </button>
      )}

      {/* Action buttons */}
      <div style={{display:"flex",gap:"8px",flexWrap:"wrap",justifyContent:"center"}}>
        <button onClick={printCard} style={{
          background:"linear-gradient(135deg,#1a1a1a,#333)",color:"white",
          border:"1px solid rgba(212,175,55,0.4)",padding:"10px 18px",
          borderRadius:"9px",cursor:"pointer",fontSize:"12px",fontWeight:"bold"
        }}>🖨️ Print</button>

        <button onClick={downloadCard} disabled={loading} style={{
          background:"linear-gradient(135deg,#007A3D,#004D1A)",color:"white",
          border:"none",padding:"10px 18px",borderRadius:"9px",cursor:"pointer",
          fontSize:"12px",fontWeight:"bold",opacity:loading?0.7:1
        }}>⬇️ Download PNG</button>

        <button onClick={sendToWhatsApp} disabled={loading} style={{
          background:"linear-gradient(135deg,#25D366,#128C7E)",color:"white",
          border:"none",padding:"10px 18px",borderRadius:"9px",cursor:"pointer",
          fontSize:"12px",fontWeight:"bold",opacity:loading?0.7:1
        }}>💬 WhatsApp پر بھیجیں</button>

        <button onClick={onClose} style={{
          background:"rgba(255,255,255,0.07)",color:"rgba(255,255,255,0.7)",
          border:"1px solid rgba(255,255,255,0.15)",padding:"10px 18px",
          borderRadius:"9px",cursor:"pointer",fontSize:"12px"
        }}>✕ بند کریں</button>
      </div>

      <div style={{fontSize:"10px",color:"rgba(255,255,255,0.3)",textAlign:"center",maxWidth:"400px"}}>
        💡 Download PNG → پلاسٹک کارڈ پرنٹنگ کے لیے تصویر پرنٹ شاپ لے جائیں | WhatsApp → تصویر ڈاؤنلوڈ ہوگی پھر نمبر کھلے گا
      </div>
    </div>
  );
}

/* ════════════════════════════════════════════════════════════════════════════
   MAIN APP
════════════════════════════════════════════════════════════════════════════ */
export default function App() {
  const [tab,setTab]=useState(0);
  const [members,setMembers]=useState([]);
  const [viewCard,setViewCard]=useState(null);
  const [adminUnlocked,setAdminUnlocked]=useState(false);
  const [adminPass,setAdminPass]=useState("");
  const [submitted,setSubmitted]=useState(false);
  const [submittedMember,setSubmittedMember]=useState(null);
  const [search,setSearch]=useState("");
  const photoRef=useRef();

  const emptyForm={
    name:"",father:"",cnic:"",mobile:"",whatsapp:"",
    area:"",address:"",role:"",
    twitter:"",facebook:"",instagram:"",youtube:"",
    joinedPPP:"",isNew:false,prevParty:"",prevRole:"",prevSince:"",
    photo:null,photoPreview:null
  };
  const [form,setForm]=useState(emptyForm);

  useEffect(()=>{
    (async()=>{try{const r=await window.storage.get("ppp-ps101-v2");if(r)setMembers(JSON.parse(r.value));}catch{}})();
    loadHtml2Canvas(); // preload
  },[]);

  async function saveMembers(list){
    try{await window.storage.set("ppp-ps101-v2",JSON.stringify(list));}catch{}
    setMembers(list);
  }

  function handlePhoto(e){
    const file=e.target.files[0];if(!file)return;
    const reader=new FileReader();
    reader.onload=ev=>setForm(f=>({...f,photo:ev.target.result,photoPreview:ev.target.result}));
    reader.readAsDataURL(file);
  }
  function setField(k,v){setForm(f=>({...f,[k]:v}));}

  function handleSubmit(){
    const missing=[];
    if(!form.name)missing.push("• پورا نام");
    if(!form.mobile)missing.push("• موبائل نمبر");
    if(!form.area)missing.push("• علاقہ");
    if(!form.role)missing.push("• PPP میں کردار");
    if(missing.length>0){alert("⚠ لازمی خانے خالی ہیں:\n\n"+missing.join("\n"));return;}
    const member={...form,memberID:genMemberID(),joinDate:todayUrdu(),registeredAt:new Date().toISOString()};
    const updated=[member,...members];
    saveMembers(updated);
    setSubmittedMember(member);
    setSubmitted(true);
    setForm(emptyForm);
  }

  function updateMember(updated){
    const list=members.map(m=>m.memberID===updated.memberID?updated:m);
    saveMembers(list);
    setViewCard(updated);
  }

  const filtered=members.filter(m=>
    m.name?.includes(search)||m.mobile?.includes(search)||
    m.memberID?.includes(search)||m.area?.includes(search)
  );

  // ── Shared styles ──────────────────────────────────────────────────────────
  const inp={width:"100%",padding:"10px 12px",borderRadius:"8px",border:"1.5px solid #d0d0d0",
    fontSize:"14px",fontFamily:"'Noto Nastaliq Urdu','Georgia',serif",direction:"rtl",boxSizing:"border-box",marginTop:"4px",outline:"none"};
  const lbl={display:"block",fontSize:"13px",fontWeight:"bold",color:"#1a1a1a",marginBottom:"2px",
    direction:"rtl",fontFamily:"'Noto Nastaliq Urdu','Georgia',serif"};
  const sec={background:"white",borderRadius:"12px",padding:"20px",marginBottom:"16px",
    border:"1.5px solid #e0e0e0",boxShadow:"0 2px 8px rgba(0,0,0,0.05)"};
  const sh=(label)=>(
    <div style={{fontSize:"15px",fontWeight:"bold",color:C.red,marginBottom:"16px",
      paddingBottom:"8px",borderBottom:"2px solid #eee"}}>{label}</div>
  );

  return (
    <div style={{minHeight:"100vh",background:"#f0f0f0",fontFamily:"'Noto Nastaliq Urdu','Georgia',serif",direction:"rtl"}}>

      {/* HEADER */}
      <div style={{background:C.black}}>
        <div style={{display:"flex",height:"7px",direction:"ltr"}}>
          <div style={{flex:1,background:"linear-gradient(to right,#8B0000,#C8102E)"}}/>
          <div style={{flex:1,background:C.black}}/>
          <div style={{flex:1,background:"linear-gradient(to left,#004D1A,#007A3D)"}}/>
        </div>
        <div style={{height:"1px",background:`linear-gradient(to right,transparent,${C.gold},transparent)`}}/>
        <div style={{maxWidth:"700px",margin:"0 auto",padding:"14px 20px",
          display:"flex",alignItems:"center",justifyContent:"space-between",gap:"12px"}}>
          <div style={{textAlign:"right"}}>
            <div style={{color:C.gold,fontSize:"18px",fontWeight:"bold"}}>پاکستان پیپلز پارٹی</div>
            <div style={{color:"white",fontSize:"12px",marginTop:"2px"}}>PS-101 ضلع شرقی کراچی</div>
            <div style={{color:C.red,fontSize:"11px",marginTop:"2px"}}>سوشل میڈیا ونگ ممبرشپ پورٹل</div>
          </div>
          <PPPFlagSVG w={96} h={64} round={7}/>
        </div>
        <div style={{height:"1px",background:`linear-gradient(to right,transparent,${C.gold},transparent)`}}/>
      </div>

      {/* TABS */}
      <div style={{maxWidth:"700px",margin:"0 auto",display:"flex",gap:"4px",padding:"12px 20px 0",direction:"ltr"}}>
        {TABS.map((t,i)=>(
          <button key={i} onClick={()=>{setTab(i);setSubmitted(false);}} style={{
            flex:1,padding:"10px 8px",border:"none",borderRadius:"8px 8px 0 0",cursor:"pointer",
            fontSize:"13px",fontWeight:"bold",background:tab===i?"white":"#ddd",
            color:tab===i?C.red:"#666",borderBottom:tab===i?`3px solid ${C.red}`:"3px solid transparent"
          }}>{t}</button>
        ))}
      </div>

      <div style={{maxWidth:"700px",margin:"0 auto",padding:"0 20px 40px"}}>

        {/* ══ TAB 0: FORM ══ */}
        {tab===0&&(submitted?(
          <div style={{...sec,textAlign:"center",padding:"40px 20px",marginTop:"16px"}}>
            <div style={{fontSize:"50px"}}>✅</div>
            <div style={{fontSize:"20px",color:C.green,fontWeight:"bold",marginTop:"12px"}}>ممبرشپ مکمل ہو گئی!</div>
            <div style={{background:"#f0faf5",border:`1.5px solid ${C.green}`,borderRadius:"10px",
              padding:"16px",margin:"16px 0",fontSize:"13px",color:"#333"}}>
              <div>ممبر ID: <strong style={{color:C.red,fontFamily:"monospace"}}>{submittedMember?.memberID}</strong></div>
              <div style={{marginTop:"6px"}}>نام: <strong>{submittedMember?.name}</strong></div>
            </div>
            <div style={{display:"flex",gap:"10px",justifyContent:"center",flexWrap:"wrap"}}>
              <button onClick={()=>setViewCard(submittedMember)} style={{
                background:C.black,color:C.gold,border:`1.5px solid ${C.gold}`,
                padding:"11px 22px",borderRadius:"8px",cursor:"pointer",fontSize:"14px",fontWeight:"bold"
              }}>🪪 ممبرشپ کارڈ دیکھیں</button>
              <button onClick={()=>setSubmitted(false)} style={{
                background:C.red,color:"white",border:"none",
                padding:"11px 22px",borderRadius:"8px",cursor:"pointer",fontSize:"14px",fontWeight:"bold"
              }}>➕ نئی ممبرشپ</button>
            </div>
          </div>
        ):(
          <>
            <div style={{...sec,marginTop:"16px"}}>
              {sh("📸 ذاتی معلومات")}
              {/* Photo Upload */}
              <div style={{display:"flex",justifyContent:"center",marginBottom:"20px"}}>
                <div style={{textAlign:"center"}}>
                  <div style={{width:"110px",height:"122px",borderRadius:"10px",
                    border:`2px solid ${form.photoPreview?C.green:"#ccc"}`,overflow:"hidden",
                    margin:"0 auto",background:"#f0f0f0",display:"flex",alignItems:"center",justifyContent:"center"}}>
                    {form.photoPreview
                      ?<img src={form.photoPreview} style={{width:"100%",height:"100%",objectFit:"cover"}}/>
                      :<div style={{textAlign:"center",color:"#bbb"}}>
                        <div style={{fontSize:"40px"}}>👤</div>
                        <div style={{fontSize:"10px",marginTop:"4px"}}>کوئی تصویر نہیں</div>
                      </div>
                    }
                  </div>
                  <label htmlFor="photoUpload" style={{
                    display:"inline-block",marginTop:"10px",
                    background:`linear-gradient(135deg,${C.green},#004D1A)`,
                    color:"white",padding:"9px 18px",borderRadius:"8px",
                    cursor:"pointer",fontSize:"13px",fontWeight:"bold",
                    boxShadow:"0 3px 10px rgba(0,122,61,0.4)"
                  }}>📷 تصویر اپلوڈ کریں</label>
                  <input id="photoUpload" ref={photoRef} type="file" accept="image/*"
                    style={{display:"none"}} onChange={handlePhoto}/>
                  {form.photoPreview&&(
                    <button onClick={()=>setForm(f=>({...f,photo:null,photoPreview:null}))} style={{
                      display:"block",margin:"5px auto 0",background:"none",border:"none",
                      color:C.red,fontSize:"11px",cursor:"pointer"
                    }}>✕ ہٹائیں</button>
                  )}
                  <div style={{fontSize:"10px",color:"#888",marginTop:"3px"}}>پاسپورٹ سائز تصویر</div>
                </div>
              </div>

              <div style={{display:"grid",gridTemplateColumns:"1fr 1fr",gap:"12px"}}>
                {[["پورا نام *","name","محمد احمد",false],["والد کا نام","father","محمد اکبر",false],
                  ["شناختی کارڈ","cnic","42101-xxxxxxx-x",true],["موبائل نمبر *","mobile","0311-xxxxxxx",true],
                  ["واٹس ایپ","whatsapp","0311-xxxxxxx",true]
                ].map(([label,key,ph,ltr])=>(
                  <div key={key}>
                    <label style={lbl}>{label}</label>
                    <input style={{...inp,direction:ltr?"ltr":"rtl",textAlign:"right"}}
                      value={form[key]} onChange={e=>setField(key,e.target.value)} placeholder={ph}/>
                  </div>
                ))}
                {/* Area - open datalist */}
                <div>
                  <label style={lbl}>علاقہ * <span style={{fontSize:"10px",color:C.red,fontWeight:"normal"}}>(لازمی)</span></label>
                  <input style={{...inp,borderColor:form.area?"#ccc":"#ffaaaa"}}
                    list="areaList" value={form.area}
                    onChange={e=>setField("area",e.target.value)}
                    placeholder="اپنا علاقہ لکھیں یا منتخب کریں"/>
                  <datalist id="areaList">{AREAS.map(a=><option key={a} value={a}/>)}</datalist>
                  {!form.area&&<div style={{fontSize:"10px",color:C.red,marginTop:"3px"}}>⚠ لازمی ہے</div>}
                </div>
              </div>
              <div style={{marginTop:"12px"}}>
                <label style={lbl}>مکمل پتہ</label>
                <textarea style={{...inp,height:"60px",resize:"vertical"}}
                  value={form.address} onChange={e=>setField("address",e.target.value)} placeholder="گھر نمبر، گلی، محلہ"/>
              </div>
            </div>

            <div style={sec}>
              {sh("🏅 پارٹی کردار")}
              <div style={{display:"grid",gridTemplateColumns:"1fr 1fr",gap:"12px"}}>
                {/* Role - open datalist */}
                <div>
                  <label style={lbl}>PPP میں کردار * <span style={{fontSize:"10px",color:C.red,fontWeight:"normal"}}>(لازمی)</span></label>
                  <input style={{...inp,borderColor:form.role?"#ccc":"#ffaaaa"}}
                    list="roleList" value={form.role}
                    onChange={e=>setField("role",e.target.value)}
                    placeholder="کردار لکھیں یا فہرست سے منتخب کریں"/>
                  <datalist id="roleList">{ROLES.map(r=><option key={r} value={r}/>)}</datalist>
                  {!form.role&&<div style={{fontSize:"10px",color:C.red,marginTop:"3px"}}>⚠ لازمی ہے</div>}
                </div>
                <div>
                  <label style={lbl}>شمولیت کب سے</label>
                  <input style={{...inp,direction:"ltr",textAlign:"right"}} type="month"
                    value={form.joinedPPP} onChange={e=>setField("joinedPPP",e.target.value)}/>
                </div>
              </div>
              <div style={{marginTop:"14px",background:"#fffbf0",borderRadius:"8px",padding:"12px",border:"1px solid #f0d080"}}>
                <label style={{...lbl,color:"#856404",display:"flex",alignItems:"center",gap:"8px",cursor:"pointer"}}>
                  <input type="checkbox" checked={form.isNew} onChange={e=>setField("isNew",e.target.checked)} style={{width:"16px",height:"16px"}}/>
                  کیا آپ نے کسی اور پارٹی سے PPP میں شمولیت اختیار کی؟
                </label>
                {form.isNew&&(
                  <div style={{display:"grid",gridTemplateColumns:"1fr 1fr",gap:"10px",marginTop:"12px"}}>
                    <div>
                      <label style={lbl}>پچھلی پارٹی</label>
                      <select style={inp} value={form.prevParty} onChange={e=>setField("prevParty",e.target.value)}>
                        <option value="">-- منتخب کریں --</option>
                        {PARTIES.map(p=><option key={p} value={p}>{p}</option>)}
                      </select>
                    </div>
                    <div>
                      <label style={lbl}>پچھلا عہدہ</label>
                      <input style={inp} value={form.prevRole} onChange={e=>setField("prevRole",e.target.value)} placeholder="مثلاً یونٹ صدر"/>
                    </div>
                    <div>
                      <label style={lbl}>کب سے</label>
                      <input style={{...inp,direction:"ltr",textAlign:"right"}} type="month"
                        value={form.prevSince} onChange={e=>setField("prevSince",e.target.value)}/>
                    </div>
                  </div>
                )}
              </div>
            </div>

            <div style={sec}>
              {sh("📱 سوشل میڈیا اکاؤنٹس")}
              <div style={{display:"grid",gridTemplateColumns:"1fr 1fr",gap:"12px"}}>
                {[["𝕏 Twitter","twitter","@username"],["Facebook","facebook","fb.com/username"],
                  ["Instagram","instagram","@username"],["YouTube","youtube","@channel"]
                ].map(([label,key,ph])=>(
                  <div key={key}>
                    <label style={lbl}>{label}</label>
                    <input style={{...inp,direction:"ltr"}} value={form[key]}
                      onChange={e=>setField(key,e.target.value)} placeholder={ph}/>
                  </div>
                ))}
              </div>
            </div>

            <button onClick={handleSubmit} style={{
              width:"100%",padding:"16px",
              background:`linear-gradient(135deg,${C.green},#004D1A)`,
              color:"white",border:"none",borderRadius:"12px",cursor:"pointer",
              fontSize:"18px",fontWeight:"bold",letterSpacing:"1px",
              boxShadow:"0 4px 20px rgba(0,122,61,0.5)"
            }}>🎉 ممبرشپ جمع کروائیں</button>
          </>
        ))}

        {/* ══ TAB 1: MEMBERS ══ */}
        {tab===1&&(
          <div style={{...sec,marginTop:"16px"}}>
            <div style={{display:"flex",justifyContent:"space-between",alignItems:"center",marginBottom:"12px",flexWrap:"wrap",gap:"8px"}}>
              <div style={{fontSize:"15px",fontWeight:"bold",color:C.red}}>
                👥 کل ممبران: <span style={{color:C.green}}>{members.length}</span>
              </div>
              <input style={{...inp,width:"200px",marginTop:"0"}}
                placeholder="🔍 نام یا نمبر تلاش" value={search} onChange={e=>setSearch(e.target.value)}/>
            </div>
            {filtered.length===0
              ?<div style={{textAlign:"center",color:"#aaa",padding:"40px"}}>کوئی ممبر نہیں ملا</div>
              :filtered.map((m,i)=>(
                <div key={i} style={{background:"#fafafa",borderRadius:"10px",padding:"12px",
                  marginBottom:"10px",border:"1px solid #e8e8e8",display:"flex",alignItems:"center",gap:"12px"}}>
                  <div style={{width:"48px",height:"54px",borderRadius:"8px",overflow:"hidden",
                    border:`2px solid ${C.gold}`,flexShrink:0,background:"#eee",
                    display:"flex",alignItems:"center",justifyContent:"center"}}>
                    {m.photo?<img src={m.photo} style={{width:"100%",height:"100%",objectFit:"cover"}}/>:<span style={{fontSize:"22px"}}>👤</span>}
                  </div>
                  <div style={{flex:1,minWidth:0}}>
                    <div style={{fontWeight:"bold",fontSize:"14px"}}>{m.name}</div>
                    <div style={{fontSize:"11px",color:C.red}}>{m.role}</div>
                    <div style={{fontSize:"10px",color:"#888",marginTop:"2px"}}>📱 {m.mobile} | 📍 {m.area}</div>
                    <div style={{fontSize:"9px",color:C.green,fontFamily:"monospace"}}>{m.memberID}</div>
                  </div>
                  <button onClick={()=>setViewCard(m)} style={{
                    background:C.black,color:C.gold,border:`1px solid ${C.gold}`,
                    padding:"8px 10px",borderRadius:"8px",cursor:"pointer",fontSize:"11px",fontWeight:"bold",whiteSpace:"nowrap"
                  }}>🪪 کارڈ</button>
                </div>
              ))
            }
          </div>
        )}

        {/* ══ TAB 2: ADMIN ══ */}
        {tab===2&&(
          <div style={{...sec,marginTop:"16px"}}>
            {!adminUnlocked?(
              <div style={{textAlign:"center",padding:"30px 20px"}}>
                <div style={{fontSize:"40px"}}>🔒</div>
                <div style={{fontSize:"16px",fontWeight:"bold",margin:"12px 0",color:C.red}}>ایڈمن پینل</div>
                <input type="password" style={{...inp,maxWidth:"250px",margin:"0 auto",display:"block",direction:"ltr"}}
                  placeholder="پاس ورڈ" value={adminPass} onChange={e=>setAdminPass(e.target.value)}
                  onKeyDown={e=>e.key==="Enter"&&(adminPass===ADMIN_PASS?setAdminUnlocked(true):alert("غلط پاس ورڈ"))}/>
                <button onClick={()=>adminPass===ADMIN_PASS?setAdminUnlocked(true):alert("غلط پاس ورڈ")} style={{
                  marginTop:"12px",background:C.red,color:"white",border:"none",
                  padding:"10px 30px",borderRadius:"8px",cursor:"pointer",fontSize:"14px",fontWeight:"bold"
                }}>داخل ہوں</button>
              </div>
            ):(
              <>
                <div style={{display:"flex",justifyContent:"space-between",alignItems:"center",marginBottom:"16px"}}>
                  <div style={{fontSize:"15px",fontWeight:"bold",color:C.red}}>🔓 ایڈمن — کل: {members.length}</div>
                  <button onClick={()=>{
                    const csv=[["Member ID","Name","Mobile","Area","Role","Twitter","Facebook","Instagram","YouTube","Since","Date"],
                      ...members.map(m=>[m.memberID,m.name,m.mobile,m.area,m.role,
                        m.twitter||"",m.facebook||"",m.instagram||"",m.youtube||"",
                        m.joinedPPP||"",m.joinDate])
                    ].map(r=>r.join(",")).join("\n");
                    const a=document.createElement("a");
                    a.href="data:text/csv;charset=utf-8,\uFEFF"+encodeURIComponent(csv);
                    a.download="PPP-PS101-Members.csv";a.click();
                  }} style={{background:C.green,color:"white",border:"none",padding:"8px 16px",borderRadius:"8px",cursor:"pointer",fontSize:"12px",fontWeight:"bold"}}>📥 CSV ڈاؤنلوڈ</button>
                </div>
                <div style={{display:"grid",gridTemplateColumns:"repeat(3,1fr)",gap:"8px",marginBottom:"16px"}}>
                  {[["سوشل میڈیا",members.filter(m=>m.role==="سوشل میڈیا ایکٹیوسٹ").length,C.red],
                    ["ورکر",members.filter(m=>m.role==="ورکر / کارکن").length,C.green],
                    ["نئے ممبران",members.filter(m=>m.isNew).length,C.gold],
                  ].map(([l,n,clr])=>(
                    <div key={l} style={{background:"white",borderRadius:"8px",padding:"12px",border:`2px solid ${clr}`,textAlign:"center"}}>
                      <div style={{fontSize:"22px",fontWeight:"bold",color:clr}}>{n}</div>
                      <div style={{fontSize:"10px",color:"#666",marginTop:"2px"}}>{l}</div>
                    </div>
                  ))}
                </div>
                {members.map((m,i)=>(
                  <div key={i} style={{background:"#fafafa",borderRadius:"8px",padding:"12px",marginBottom:"8px",
                    border:"1px solid #eee",display:"flex",alignItems:"center",gap:"10px"}}>
                    <div style={{width:"40px",height:"44px",borderRadius:"6px",overflow:"hidden",
                      border:`1.5px solid ${C.gold}`,flexShrink:0,background:"#eee",
                      display:"flex",alignItems:"center",justifyContent:"center"}}>
                      {m.photo?<img src={m.photo} style={{width:"100%",height:"100%",objectFit:"cover"}}/>:<span>👤</span>}
                    </div>
                    <div style={{flex:1,fontSize:"12px"}}>
                      <div style={{fontWeight:"bold"}}>{m.name} | <span style={{color:C.red}}>{m.role}</span></div>
                      <div style={{color:"#888"}}>{m.mobile} | {m.area}</div>
                      <div style={{color:C.green,fontFamily:"monospace",fontSize:"10px"}}>{m.memberID}</div>
                    </div>
                    <div style={{display:"flex",gap:"4px",flexDirection:"column"}}>
                      <button onClick={()=>setViewCard(m)} style={{
                        background:C.black,color:C.gold,border:"none",
                        padding:"5px 8px",borderRadius:"6px",cursor:"pointer",fontSize:"10px"
                      }}>🪪 کارڈ</button>
                      <button onClick={()=>{
                        if(confirm("کیا آپ واقعی اس ممبر کو حذف کرنا چاہتے ہیں؟")){
                          saveMembers(members.filter((_,j)=>j!==i));
                        }
                      }} style={{background:"#fee",color:C.red,border:"1px solid #fcc",
                        padding:"5px 8px",borderRadius:"6px",cursor:"pointer",fontSize:"10px"}}>🗑️</button>
                    </div>
                  </div>
                ))}
              </>
            )}
          </div>
        )}
      </div>

      {viewCard&&<MemberCard member={viewCard} onClose={()=>setViewCard(null)} onUpdateMember={updateMember}/>}

      <div style={{background:C.black,padding:"0"}}>
        <div style={{display:"flex",height:"4px",direction:"ltr"}}>
          <div style={{flex:1,background:C.red}}/><div style={{flex:1,background:"#222"}}/><div style={{flex:1,background:C.green}}/>
        </div>
        <div style={{color:"rgba(255,255,255,0.2)",fontSize:"9px",textAlign:"center",padding:"8px",letterSpacing:"1px",direction:"ltr"}}>
          PAKISTAN PEOPLES PARTY — PS-101 Social Media Wing — District East Karachi
        </div>
      </div>
    </div>
  );
}
