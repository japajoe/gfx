#include "CharacterDemo.hpp"
#include "PrefabUtility.hpp"

void CharacterDemo::OnInitialize()
{
	World::SetFogEnabled(false);

	auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox2);
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	auto orbit = camera->GetGameObject()->AddComponent<MouseOrbit>();
	camera->GetGameObject()->GetTransform()->SetPosition(Vector3(0, 2, 10));

	auto lightTransform = Light::GetMain()->GetTransform();
	lightTransform->SetRotation(Quaternionf::Euler(glm::radians(5.0f), 0, 0));
	
	auto material = skybox->GetComponent<MeshRenderer>()->GetMaterial<ProceduralSkybox2Material>(0);
	material->SetSunPositionFromLightDirection(lightTransform->GetForward());
	material->SetExposure(0.3f);

	PrefabUtility::CreateGround();
	PrefabUtility::CreateCubes();

	player = PrefabUtility::CreateCharacter();
	player->GetTransform()->SetPosition(Vector3(0, 125.0f, 0));
	auto mesh = player->GetComponent<MeshRenderer>()->GetMesh(0);
	bounds = mesh->GetBounds();

	auto ballObject= PrefabUtility::CreateBall();
	ball = ballObject->GetComponent<Rigidbody>();

	orbit->SetTarget(player->GetTransform());
	orbit->zAxisDistance *= 0.5f;
}

void CharacterDemo::OnUpdate()
{
	if(Input::GetKeyDown(KeyCode::Escape))
	{
		Application::Quit();
	}

	if(Input::GetKeyDown(KeyCode::C))
	{
		Input::SetMouseCursor(!Input::IsCursorVisible());
	}

	if(Input::GetButtonDown(ButtonCode::Left))
	{
		ball->SetLinearVelocity(Vector3f::Zero());
		ball->SetAngularVelocity(Vector3f::Zero());
		ball->MovePosition(player->GetTransform()->GetPosition());
		ball->SetLinearVelocity(Camera::GetMain()->GetTransform()->GetForward() * 50.0f);
	}

	GetInput();
	Rotate();
	Move();
}

void CharacterDemo::OnLateUpdate()
{
	std::string fps = "FPS " + std::to_string(Time::GetFPS());
	GUILayout::BeginFrame();
	GUILayout::Text(fps);
	GUILayout::CheckBox(showBounds);
	GUILayout::SameLine();
	float y = GUILayout::GetCursorY() - 4;
	GUILayout::SetCursorY(y);
	GUILayout::Text("Show Bounding Box");
	GUILayout::EndFrame();

	if(showBounds)
		DrawBounds();
}

void CharacterDemo::OnGUI()
{

}

void CharacterDemo::GetInput()
{
	vertical = Input::GetAxis("Vertical");
	horizontal = Input::GetAxis("Horizontal");
	isJumping = Input::GetKeyDown(KeyCode::Space);
}

void CharacterDemo::Rotate()
{
	auto transform = player->GetTransform();
	auto camera = Camera::GetMain()->GetTransform();

	// Get the camera's forward and right vectors, projected onto the XZ plane.
	Vector3 cameraForward = camera->GetForward();
	cameraForward.y = 0;
	cameraForward = Vector3f::Normalize(cameraForward);

	Vector3 cameraRight = camera->GetRight();
	cameraRight.y = 0;
	cameraRight = Vector3f::Normalize(cameraRight);

	// Calculate the movement direction based on the camera's orientation.
	movementDirection = (cameraForward * vertical) + (cameraRight * horizontal);
	
	//Normalize movement direction if it's not zero.
	if(Vector3f::LengthSquared(movementDirection) > 0.0f)
	{
		movementDirection = Vector3f::Normalize(movementDirection);
	}

	// Rotate the player to the direction of movement
	if (Vector3f::LengthSquared(movementDirection) > 0.0f)
	{
		Quaternion targetRotation = Quaternionf::LookRotation(movementDirection, Vector3f::UnitY());
		player->GetTransform()->SetRotation(Quaternionf::Slerp(player->GetTransform()->GetRotation(), targetRotation, 0.15f));
	}
}

static bool IsBumpingIntoObstacles(const Transform *transform, const Vector3 &direction, float sphereCastDistance, float sphereCastRadius)
{
	RaycastHit hit;
	Vector3 sphereCastOrigin = transform->GetPosition() + Vector3f::Up() * 0.5f; // Start the sphere cast a bit above the player's position
	return Physics::SphereTest(sphereCastOrigin, direction, sphereCastDistance, sphereCastRadius, hit);
}

void CharacterDemo::Move()
{
    const float gravity = -9.8f;
    const float groundCheckRadius = 0.6f;
    const float sphereCastRadius = 0.3f;
    const float sphereCastDistance = 0.5f;

	// Check if we are grounded using SphereCheck
	auto transform = player->GetTransform();
	const bool isGrounded = Physics::CheckSphere(transform->GetPosition() + -Vector3f::Up() * groundCheckRadius, groundCheckRadius);
	auto newPosition = transform->GetPosition();

	// Apply gravity if not grounded
	if (!isGrounded)
	{
		verticalVelocity += gravity * Time::GetDeltaTime();
	}
	else
	{
		verticalVelocity = 0.0f;

		// Handle jumping
		if (isJumping)
		{
			verticalVelocity += Mathf::Sqrt(jumpForce * -2.0f * gravity);
		}
	}

	// Apply SphereCast to check for obstacles before moving
	
	Vector3 slideDirection;
	if (!IsBumpingIntoObstacles(transform, movementDirection, sphereCastDistance, sphereCastRadius))
	{
		// Apply movement
		newPosition += movementDirection * speed * Time::GetDeltaTime();
	}
	// else
	// {
	// 	newPosition += slideDirection * speed * Time::GetDeltaTime();
	// }

	Vector3 velocity(0, verticalVelocity, 0);

	newPosition += velocity * Time::GetDeltaTime();
	transform->SetPosition(newPosition);
}

void CharacterDemo::DrawBounds()
{
	auto transform = player->GetTransform();
	Vector3 position = transform->GetPosition();
	Quaternion rotation = transform->GetRotation();
	Vector3 min = bounds.GetMin();
	Vector3 max = bounds.GetMax();
	Debug::DrawBounds(min, max, position, rotation, Color::White());
}

static Vector3 ProjectAndScale(const Vector3 &vec, const Vector3 &normal)
{
	float magnitude = Vector3f::Length(vec);
	Vector3 v = Vector3f::Normalize(Vector3f::ProjectOnPlane(vec, normal));
	v *= magnitude;
	return v;
}

Vector3 CharacterDemo::CollideAndSlide(const Vector3 &position, const Vector3 &velocity, const Vector3 &initialVelocity, bool gravityPass, bool isGrounded, int depth)
{
	if(depth > maxBounces)
		return Vector3f::Zero();
	
	float distance = Vector3f::Length(velocity) + skinWidth;

	RaycastHit hit;
	Vector3 direction = Vector3f::Normalize(velocity);
	if(Physics::SphereTest(position, direction, distance, bounds.GetExtents().x, hit))
	{
		Vector3 snapToSurface = direction * (hit.distance - skinWidth);
		Vector3 leftOver = velocity - snapToSurface;
		float angle = Vector3f::Angle(Vector3f::Up(), hit.normal);

		if(Vector3f::Length(snapToSurface) <= skinWidth)
		{
			snapToSurface = Vector3f::Zero();
		}

		//Normal or ground slope
		if(angle <= maxSlopeAngle)
		{
			if(gravityPass)
				return snapToSurface;
			leftOver = ProjectAndScale(leftOver, hit.normal);
		}
		else //Wall or steep slope
		{
			float scale = 1.0f - Vector3f::Dot( Vector3f::Normalize(Vector3(hit.normal.x, 0, hit.normal.z)),
											   -Vector3f::Normalize(Vector3(initialVelocity.x, 0, initialVelocity.z)));

			if(isGrounded && gravityPass)
			{
				leftOver = Vector3f::ProjectOnPlane(Vector3(leftOver.x, 0, leftOver.z),
													Vector3(hit.normal.x, 0, hit.normal.z));
				leftOver = Vector3f::Normalize(leftOver) * scale;
			}
			else
			{
				leftOver = Vector3f::ProjectOnPlane(leftOver, hit.normal) * scale;
			}

		}

		return snapToSurface + CollideAndSlide(leftOver, position + snapToSurface, initialVelocity, gravityPass, isGrounded, depth + 1);
	}
	return velocity;
}