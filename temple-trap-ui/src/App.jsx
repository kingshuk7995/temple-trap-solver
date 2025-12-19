function BottomL({ rotation = "0deg" }) {
  return (
    <div
      className="w-full h-full flex flex-col"
      style={{ transform: `rotate(${rotation})` }}
    >
      {/* Top empty area */}
      <div className="flex flex-1">
        {/* Left vertical bar */}
        <div className="w-1/4 bg-amber-200" />
        {/* Top-right filler */}
        <div className="flex-1 bg-white" />
      </div>

      {/* Bottom horizontal bar */}
      <div className="h-1/4 w-full bg-amber-200" />
    </div>
  );
}


export default function App() {
  return (
    <div className="w-64 h-64 border border-black">
      <BottomL rotation="0deg" />
    </div>
  );
}
